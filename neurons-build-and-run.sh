#!/bin/bash

premake5 gmake2

if [ $? -ne 0 ]; then
    echo "premake5 gmake2 failed"
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

cp -r Neurons/Assets out/bin/Debug-linux-x86_64/Neurons

if [ $? -ne 0 ]; then
    echo "Failed to copy Assets directory to out/bin/Debug-linux-x86_64/Neurons"
    exit 1
fi

cd out/bin/Debug-linux-x86_64/Neurons

if [ $? -ne 0 ]; then
    echo "Failed to navigate to out/bin/Debug-linux-x86_64/Neurons"
    exit 1
fi

./Neurons

if [ $? -ne 0 ]; then
    echo "Failed to run Neurons"
    exit 1
fi