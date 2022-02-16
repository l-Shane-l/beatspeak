#!/usr/bin/gnuplot -persist
set key autotitle columnheader
set datafile separator ','
set key off
plot for [i=1:10] "../test_data/pre_spline.dat" using 0:i with linespoints; pause -1 
plot for [i=1:10] "../test_data/post_spline.dat" using 0:i with linespoints; pause -1 


