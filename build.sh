#!/bin/bash

gcc main.c  lib/librarylib.a -o Pathfinding.exe -o1 -Wall -std=c99 -Wno-missing-braces -Iinlcude/ -Ilib/ -lopengl32 -lgdi32 -lwinmm
.\1.exe