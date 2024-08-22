#!/bin/bash


# Uninstall script for moused

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Stop the moused service
# Disable the moused service from starting on boot
echo "Stopping moused service"
systemctl stop moused
systemctl disable moused

# Remove the moused script from /usr/bin
echo "Removing moused from /usr/bin"
rm /usr/bin/moused

# Remove the moused.service file from /etc/systemd/system
echo "Removing moused.service from /etc/systemd/system"
rm /etc/systemd/system/moused.service

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

echo "moused uninstalled successfully"

