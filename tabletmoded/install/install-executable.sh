#!/bin/bash

# Change to the directory of the script
cd "$(dirname "$0")"

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Copy the tabletmoded to $DESTDIR
echo "Copying tabletmoded to $DESTDIR"
install -Dm755 ../bin/tabletmoded $DESTDIR/tabletmoded
