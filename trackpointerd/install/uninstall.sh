#!/bin/bash


# Uninstall script for trackpointerd

# Check if the script is run as root
# If not, exit with error

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Stop the trackpointerd service
# Disable the trackpointerd service from starting on boot
echo "Stopping trackpointerd service"
systemctl stop trackpointerd
systemctl disable trackpointerd

# Remove the trackpointerd script from /usr/bin
echo "Removing trackpointerd from /usr/bin"
rm /usr/bin/trackpointerd

# Remove the trackpointerd.service file from /etc/systemd/system
echo "Removing trackpointerd.service from /etc/systemd/system"
rm /etc/systemd/system/trackpointerd.service

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

echo "trackpointerd uninstalled successfully"

