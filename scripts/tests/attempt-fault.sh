#!/bin/bash

. common.sh

if [[ $# -ne 1 && $# -ne 2 ]] ; then
    >&2 echo "Incorrect number of arguments"
    >&2 echo "Usage: $0 <fault voltage> [original voltage]"
    >&2 echo "Both <fault voltage> and [original voltage] are voltage offsets in mV."
    >&2 echo "If [original voltage] is not specified, it defaults to 0."
    exit 1
fi

test_voltage="$1"

original_voltage=0
if [[ $# -eq 2 ]] ; then
    original_voltage="$2"
fi

undervolt --core "$test_voltage" --cache "$test_voltage"
shasum -b -a 1 ._shatest_data | (echo -n "$voltage " && cat)
undervolt --core "$original_voltage" --cache "$original_voltage"
