#!/bin/sh

g++ -c -o info_hasher.o info_hasher.cpp
g++ -c -o app.o app.cpp
g++ -o app app.o info_hasher.o -lcppbencode -lpolarssl -lkyotocabinet
