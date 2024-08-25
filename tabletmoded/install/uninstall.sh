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

# Execute the uninstall-executable.sh script
chmod +x ./uninstall-executable.sh
TARGETDIR="/usr/bin" ./uninstall-executable.sh
# Execute the uninstall-service.sh script
chmod +x ./uninstall-service.sh
TARGETDIR="/etc/systemd/system" ./uninstall-service.sh

# Reload systemd
echo "Reloading systemd"
systemctl daemon-reload

echo "tabletmoded uninstalled successfully"

