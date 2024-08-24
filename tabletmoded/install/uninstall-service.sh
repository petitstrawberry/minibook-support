#!/bin/bash

# Check if $TARGETDIR is set
if [ -z "$TARGETDIR" ]; then
    echo "Please set TARGETDIR"
    exit
fi

# Remove the tabletmoded.service from $TARGETDIR
echo "Removing tabletmoded.service from $TARGETDIR"
rm $TARGETDIR/tabletmoded.service
