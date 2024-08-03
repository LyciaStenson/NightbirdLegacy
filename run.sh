#!/bin/bash

cd out/bin/Debug-linux-x86_64/Engine

if [ $? -ne 0 ]; then
    echo "Failed to navigate to out/bin/Debug-linux-x86_64/Engine"
    exit 1
fi

./Engine

if [ $? -ne 0 ]; then
    echo "Failed to run Engine"
    exit 1
fi
