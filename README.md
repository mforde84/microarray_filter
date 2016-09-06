# microarray_filter

Determines whether or not log2 signal intensity is above 95% percentile threshold defined by a list of user specified probe IDs. 

P - PASS, M - MARIGINAL PASS (i.e., residual to threshold less than 1), F - FAIL

Usage:

./microarray_filter signal_intensities.txt controls.txt > output.txt

Compile:

g++ -o microarray_filter microarray_filter.cpp --std=c++1z

