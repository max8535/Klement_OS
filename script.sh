#!/bin/bash

gcc main.c -o main
gcc plus.c -o plus
gcc minus.c -o minus
gcc multiplication.c -o multiplication
gcc division.c -o division
gcc sqrt.c -o sqrt -lm
./main
