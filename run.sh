#!/bin/bash

cd out/bin/Debug-linux-x86_64/Nightbird

if [ $? -ne 0 ]; then
    echo "Failed to navigate to out/bin/Debug-linux-x86_64/Nightbird"
    exit 1
fi

./Nightbird

if [ $? -ne 0 ]; then
    echo "Failed to run Nightbird"
    exit 1
fi
