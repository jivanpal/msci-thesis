#!/bin/bash

. common.sh

rm -f ._shatest_*

dd if=/dev/urandom bs=4194304 count=1 > ._shatest_data
shasum -b -a 1 ._shatest_data > ._shatest_hash
