#!/bin/bash

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Copy the keyboardd.service file to $DESTDIR
echo "Copying keyboardd.service to $DESTDIR"
install  -Dm644 ./keyboardd.service $DESTDIR/keyboardd.service
