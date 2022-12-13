#!/usr/bin/gnuplot -persist
set key outside
set key off
while (1) {
plot '../build/result.dat' with lines
pause(1)
}