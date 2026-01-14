#!/bin/sh
set -e

if [ ! -d "PDCurses" ]; then
    echo "Cloning PDCurses..."
    git clone https://github.com/wmcbrine/PDCurses.git
else
    echo "PDCurses already cloned."
fi
