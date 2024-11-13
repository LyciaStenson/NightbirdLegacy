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

cp -r Editor/Assets out/bin/$1-linux-x86_64/Editor

if [ $? -ne 0 ]; then
    echo Failed to copy Assets directory to out/bin/$1-linux-x86_64/Editor
    exit 1
fi

cd out/bin/$1-linux-x86_64/Editor

if [ $? -ne 0 ]; then
    echo Failed to navigate to out/bin/$1-linux-x86_64/Editor
    exit 1
fi

./Editor

if [ $? -ne 0 ]; then
    echo "Failed to run Editor"
    exit 1
fi
