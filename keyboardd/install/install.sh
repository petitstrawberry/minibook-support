#!/bin/bash
# Install script for keyboardd as a systemd service

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Copy the keyboardd to /usr/bin
echo "Copying moused to /usr/bin" 
install -Dm755 ../bin/keyboardd /usr/bin/keyboardd

# Copy the keyboardd.service file to /etc/systemd/system
echo "Copying keyboardd.service to /etc/systemd/system"
install  -Dm644 ./keyboardd.service /etc/systemd/system/keyboardd.service 

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

# Start the moused service
# Enable the moused service to start on boot

echo "Starting keyboardd service"
systemctl enable keyboardd
systemctl start keyboardd

echo "keyboardd installed successfully"

