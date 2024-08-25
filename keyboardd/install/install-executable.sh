#!/bin/bash

# Change to the directory of the script
cd "$(dirname "$0")"

# Check if $DESTDIR is set
if [ -z "$DESTDIR" ]; then
    echo "Please set DESTDIR"
    exit
fi

# Copy the keyboardd to $DESTDIR
echo "Copying keyboardd to $DESTDIR"
install -Dm755 ../bin/keyboardd $DESTDIR/keyboardd
