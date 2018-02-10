#!/bin/bash

# Compile from source
g++ -o donationAnalytics ./src/main.cpp

# Run
./donationAnalytics ./input/percentile.txt ./input/itcont.txt ./output/repeat_donors.txt

