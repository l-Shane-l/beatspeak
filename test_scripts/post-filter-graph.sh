#!/usr/bin/gnuplot -persist
set key outside
set key off
while (1) {
plot '../build/post_filter.dat' with lines
pause(1)
}