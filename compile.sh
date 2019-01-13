#!/usr/bin/en sh
echo "compile picture"
g++ -o picture picture.cpp Common.h DarkChannel.h DarkChannel.cpp GuideFilter.h GuideFilter.cpp HazeRemoval.h HazeRemoval.cpp -std=c++11 -O3 `pkg-config --cflags --libs opencv`
echo "done"
