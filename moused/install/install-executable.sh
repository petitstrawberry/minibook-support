#!/bin/bash

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Change to the directory of the script
cd "$(dirname "$0")"

# Copy the moused to $DESTDIR
echo "Copying moused to $DESTDIR"
install -Dm755 ../bin/moused $DESTDIR/moused