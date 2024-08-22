#!/bin/bash

cd out/bin/Debug-linux-x86_64/Game

if [ $? -ne 0 ]; then
    echo "Failed to navigate to out/bin/Debug-linux-x86_64/Editor"
    exit 1
fi

./Game

if [ $? -ne 0 ]; then
    echo "Failed to run Editor"
    exit 1
fi
