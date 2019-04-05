#!/bin/bash
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
