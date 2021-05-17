#!/bin/bash

if [ ! -d "build/release" ]; then
    mkdir -p "build/release"
else
    echo "clean"
    # rm build/* -r
fi

cd build

export ISP_HW_VERSION="-DISP_HW_V21"

cmake -DCMAKE_INSTALL_PREFIX=./release -DISP_HW_VERSION=${ISP_HW_VERSION} ../


make -j4

make install

if [ $? -eq 0  ]; then
    echo Build finished!
    cd ..
fi
