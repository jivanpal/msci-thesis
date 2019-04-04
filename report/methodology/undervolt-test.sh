#!/bin/bash

## The voltage offsets to test will be fetched from
## `undervolt-list-f.txt`, where `f` is the specified
## frequency in MHz.
##
## The optional 'all' argument causes `undervolt-list.txt`
## to be used as the list of voltage offsets to test. This
## file is intended to have an exhaustive list of voltage
## offsets.

. common.sh

if [[ $# -ne 1 && $# -ne 2 ]] ; then
    >&2 echo "Incorrect number of arguments"
    >&2 echo "Usage: $0 <frequency in MHz> [all]"
    exit 1
fi

cpu_frequency="$1"

all_flag=0
if [[ $# -eq 2 && $2 == "all" ]] ; then
 all_flag=1
fi

voltages_list="undervolt-list-${cpu_frequency}.txt"
if [ $all_flag -eq 1 ] ; then
  voltages_list="undervolt-list.txt"
fi

cpupower frequency-set -f "${cpu_frequency}MHz"

while IFS= read -r voltage; do
  (
    undervolt --core "$voltage" --cache "$voltage"
    shasum -b -a 1 ._shatest_data | (echo -n "$voltage " && cat)
    undervolt --core 0 --cache 0
  )
done < "$voltages_list"
