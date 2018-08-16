#!/bin/bash
sudo apt-get install cmake libwxgtk3.0-dev
mkdir -p build
cd build && cmake .. && make
