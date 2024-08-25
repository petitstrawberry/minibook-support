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

# Execute the install-executable.sh script
chmod +x ./install-executable.sh
DESTDIR="/usr/bin" ./install-executable.sh
# Execute the install-service.sh script
chmod +x ./install-service.sh
DESTDIR="/etc/systemd/system" ./install-service.sh

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

# Start the moused service
# Enable the moused service to start on boot

echo "Starting keyboardd service"
systemctl enable keyboardd
systemctl start keyboardd

echo "keyboardd installed successfully"

