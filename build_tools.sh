#!/bin/sh
clear
mkdir build
cd build
cmake ../
make data_editor
if [ $? -ne 0 ]; then
	exit 1
fi
sleep 1.5s
cp -r ../data ./
./data_editor
