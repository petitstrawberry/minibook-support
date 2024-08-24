#!/bin/bash

# Check if $TARGETDIR is set
if [ -z "$TARGETDIR" ]; then
    echo "Please set TARGETDIR"
    exit
fi

# Remove the tabletmoded from $TARGETDIR
echo "Removing tabletmoded from $TARGETDIR"
rm $TARGETDIR/tabletmoded
