#!/bin/bash

# Check if $TARGETDIR is set
if [ -z "$TARGETDIR" ]; then
    echo "Please set TARGETDIR"
    exit
fi

# Remove the moused from $TARGETDIR
echo "Removing moused from $TARGETDIR"
rm $TARGETDIR/moused
