#!/bin/bash
# Install script for moused as a systemd service

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Copy the moused script to /usr/bin
echo "Copying moused to /usr/bin" 
install -Dm755 ../bin/moused /usr/bin/moused

# Copy the moused.service file to /etc/systemd/system
echo "Copying moused.service to /etc/systemd/system"
install  -Dm644 ./moused.service /etc/systemd/system/moused.service 

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

# Start the moused service
# Enable the moused service to start on boot

echo "Starting moused service"
systemctl enable moused
systemctl start moused

echo "moused installed successfully"

