#!/bin/bash


# Uninstall script for tabletmoded

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Stop the tabletmoded service
# Disable the tabletmoded service from starting on boot
echo "Stopping tabletmoded service"
systemctl stop tabletmoded
systemctl disable tabletmoded

# Remove the tabletmoded script from /usr/bin
echo "Removing tabletmoded from /usr/bin"
rm /usr/bin/tabletmoded

# Remove the tabletmoded.service file from /etc/systemd/system
echo "Removing tabletmoded.service from /etc/systemd/system"
rm /etc/systemd/system/tabletmoded.service

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

echo "tabletmoded uninstalled successfully"

