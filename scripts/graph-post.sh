#!/bin/bash

while true
do
    gnuplot -e "set xlabel 'Time'; set ylabel 'Value'; plot 'post_filter.txt' with lines"
    sleep 1
done