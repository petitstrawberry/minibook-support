#!/bin/bash

# Change to the directory of the script
cd "$(dirname "$0")"

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Copy the moused to $DESTDIR
echo "Copying moused to $DESTDIR"
install -Dm755 ../bin/moused $DESTDIR/moused
