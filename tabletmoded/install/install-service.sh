#!/bin/bash

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Copy the tabletmoded.service file to $DESTDIR
echo "Copying tabletmoded.service to $DESTDIR"
install  -Dm644 ./tabletmoded.service $DESTDIR/tabletmoded.service
