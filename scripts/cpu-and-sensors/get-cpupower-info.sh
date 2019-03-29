#!/bin/bash
for core in {0..3}; do
  cpupower -c "$core" frequency-info
done
