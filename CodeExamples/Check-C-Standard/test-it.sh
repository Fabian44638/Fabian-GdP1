#!/usr/bin/env bash
for std in c89 c99 c11 gnu89 gnu99 gnu11; do 
  echo $std
  gcc -std=$std -o main main.c
  ./main
  echo
done
echo default
gcc -o main main.c
./main


rm -f main

