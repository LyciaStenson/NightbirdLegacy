#!/bin/bash

premake5 gmake2

if [ $? -ne 0 ]; then
    echo "premake5 gmake2 failed"
    exit 1
fi

make config=$1

if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

cp -r Spin/Assets out/bin/$1-linux-x86_64/Spin

if [ $? -ne 0 ]; then
    echo Failed to copy Assets directory to out/bin/$1-linux-x86_64/Spin
    exit 1
fi

cd out/bin/$1-linux-x86_64/Spin

if [ $? -ne 0 ]; then
    echo Failed to navigate to out/bin/$1-linux-x86_64/Spin
    exit 1
fi

./Spin

if [ $? -ne 0 ]; then
    echo "Failed to run Spin"
    exit 1
fi
