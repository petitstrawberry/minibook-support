#!/bin/bash

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Copy the moused.service file to $DESTDIR
echo "Copying moused.service to $DESTDIR"
install  -Dm644 ./moused.service $DESTDIR/moused.service
