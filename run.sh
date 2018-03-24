#!/bin/bash
g++ -o shell *.cpp -std=c++11 -Wall -Who-vla -pedantic -march=core2 -Os -pipe -fstack-protector-all -g3 -Wl,-O,1,-z,combreloc
