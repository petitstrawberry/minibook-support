#!/bin/bash
# Install script for trackpointerd as a systemd service

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Copy the trackpointerd script to /usr/bin
echo "Copying trackpointerd to /usr/bin" 
install -Dm755 ../bin/trackpointerd /usr/bin/trackpointerd

# Copy the trackpointerd.service file to /etc/systemd/system
echo "Copying trackpointerd.service to /etc/systemd/system"
install  -Dm644 ./trackpointerd.service /etc/systemd/system/trackpointerd.service 

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

# Start the trackpointerd service
# Enable the trackpointerd service to start on boot

echo "Starting trackpointerd service"
systemctl enable trackpointerd
systemctl start trackpointerd

echo "trackpointerd installed successfully"

