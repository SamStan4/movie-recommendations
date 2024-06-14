#!/usr/bin/bash

exe_file="program"
time_file="time_file.txt"

if command -v g++ >/dev/null 2>&1; then
    if [ -f $exe_file ]; then
        rm $exe_file
    fi
    g++ main.cpp -o program
    if [ -f $exe_file ]; then
        { time ./$exe_file >> $time_file; } 2>> $time_file
    fi
else
    echo "please install the g++ compiler"
fi