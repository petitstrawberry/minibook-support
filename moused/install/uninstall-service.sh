#!/bin/bash

# Check if $TARGETDIR is set
if [ -z "$TARGETDIR" ]; then
    echo "Please set TARGETDIR"
    exit
fi

# Remove the moused.service from $TARGETDIR
echo "Removing moused.service from $TARGETDIR"
rm $TARGETDIR/moused.service
