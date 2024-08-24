#!/bin/bash

# Change to the directory of the script
cd "$(dirname "$0")"

# Check if $TARGETDIR is set
if [ -z "$TARGETDIR" ]; then
    echo "Please set TARGETDIR"
    exit
fi

# Remove the keyboardd.service from $TARGETDIR
echo "Removing keyboardd.service from $TARGETDIR"
rm $TARGETDIR/keyboardd.service
