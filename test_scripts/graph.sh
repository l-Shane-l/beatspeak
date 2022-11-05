#!/usr/bin/gnuplot -persist
set key outside
set key off
while (1) {
plot for [col=1:50] '../build/output.dat' using 0:col with lines
pause(1)
}