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

cp Engine/thisShader.vert out/bin/Debug-linux-x86_64/Engine

if [ $? -ne 0 ]; then
    echo "Failed to copy thisShader.vert to out/bin/Debug-linux-x86_64/Engine"
    exit 1
fi

cp Engine/thisShader.frag out/bin/Debug-linux-x86_64/Engine

if [ $? -ne 0 ]; then
    echo "Failed to copy thisShader.frag to out/bin/Debug-linux-x86_64/Engine"
    exit 1
fi

cp -r Engine/Assets out/bin/Debug-linux-x86_64/Engine

if [ $? -ne 0 ]; then
    echo "Failed to copy Assets directory to out/bin/Debug-linux-x86_64/Engine"
    exit 1
fi

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
