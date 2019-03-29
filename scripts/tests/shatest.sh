#!/bin/bash

. common.sh

if [ $# -ne 1 ] ; then
    echo "Incorrect number of arguments" >&2
    echo "Usage: $0 <file listing voltage offsets in mV>" >&2
    exit 1
fi

voltages_list="$1"

while IFS= read -r voltage; do
  (
    undervolt --core "$voltage" --cache "$voltage"
    shasum -b -a 1 ._shatest_data | (echo -n "$voltage " && cat)
    undervolt --core 0 --cache 0
  )
done < "$voltages_list"
