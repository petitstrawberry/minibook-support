#!/bin/bash

# Check if $TARGETDIR is set
if [ -z "$TARGETDIR" ]; then
    echo "Please set TARGETDIR"
    exit
fi

# Remove the keyboardd.service from $TARGETDIR
echo "Removing keyboardd.service from $TARGETDIR"
rm $TARGETDIR/keyboardd.service
