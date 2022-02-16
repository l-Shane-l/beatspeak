#!/bin/bash
set -e
echo "building"
pwd
cd ../build
make
echo "removing previous runs data"
pwd
rm ../test_data/*
echo "running"
pwd
timeout 10 ./source/beatspeak demo
echo "graphing"
pwd
cd ../test_scripts
pwd
./graph.sh
