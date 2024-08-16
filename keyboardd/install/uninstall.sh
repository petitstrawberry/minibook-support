#!/bin/bash


# Uninstall script for keyboardd

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Stop the keyboardd service
# Disable the keyboardd service from starting on boot
echo "Stopping keyboardd service"
systemctl stop keyboardd
systemctl disable keyboardd

# Remove the trackpointerd script from /usr/bin
echo "Removing keyboardd from /usr/bin"
rm /usr/bin/keyboardd

# Remove the keyboardd.service file from /etc/systemd/system
echo "Removing keyboardd.service from /etc/systemd/system"
rm /etc/systemd/system/keyboardd.service

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

echo "keyboardd uninstalled successfully"

