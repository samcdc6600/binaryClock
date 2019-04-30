#!/bin/sh
clang++ -std=c++1z -O3 main.cpp -lX11 -L/usr/local/lib/ -I/usr/local/include/
rm *.gch *# *~ *.core
