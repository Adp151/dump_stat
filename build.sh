#!/bin/bash

mkdir -p build
mkdir -p bin
cd build || exit

cmake ..

make

cp -f ./dump_test/dump_test ./../bin/dump_test
cp -f ./src/dump_stat ./../bin/dump_stat

cd ./../bin || exit
chmod +x dump_stat
