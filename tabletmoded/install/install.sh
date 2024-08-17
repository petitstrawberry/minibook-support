#!/bin/bash
# Install script for tabletmoded as a systemd service

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Copy the tabletmoded script to /usr/bin
echo "Copying tabletmoded to /usr/bin" 
install -Dm755 ../bin/tabletmoded /usr/bin/tabletmoded

# Copy the tabletmoded.service file to /etc/systemd/system
echo "Copying tabletmoded.service to /etc/systemd/system"
install  -Dm644 ./tabletmoded.service /etc/systemd/system/tabletmoded.service 

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

# Start the tabletmoded service
# Enable the tabletmoded service to start on boot

echo "Starting tabletmoded service"
systemctl enable tabletmoded
systemctl start tabletmoded

echo "tabletmoded installed successfully"

