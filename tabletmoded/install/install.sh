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

# Execute the install-executable.sh script
chmod +x ./install-executable.sh
DESTDIR="/usr/bin" ./install-executable.sh
# Execute the install-service.sh script
chmod +x ./install-service.sh
DESTDIR="/etc/systemd/system" ./install-service.sh

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

# Start the tabletmoded service
# Enable the tabletmoded service to start on boot

echo "Starting tabletmoded service"
systemctl enable tabletmoded
systemctl start tabletmoded

echo "tabletmoded installed successfully"
