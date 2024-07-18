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

cp Nightbird/thisShader.vert out/bin/Debug-linux-x86_64/Nightbird

if [ $? -ne 0 ]; then
    echo "Failed to copy thisShader.vert to out/bin/Debug-linux-x86_64/Nightbird"
    exit 1
fi

cp Nightbird/thisShader.frag out/bin/Debug-linux-x86_64/Nightbird

if [ $? -ne 0 ]; then
    echo "Failed to copy thisShader.frag to out/bin/Debug-linux-x86_64/Nightbird"
    exit 1
fi

cd out/bin/Debug-linux-x86_64/Nightbird

if [ $? -ne 0 ]; then
    echo "Failed to navigate to out/bin/Debug-linux-x86_64/Nightbird"
    exit 1
fi
