#!/usr/bin/gnuplot -persist
set key outside
set key off
while (1) {
plot 'result.dat' with lines
pause(1)
}