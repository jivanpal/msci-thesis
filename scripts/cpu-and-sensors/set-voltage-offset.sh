#!/bin/sh

if [ $# -ne 1 ] ; then
  >&2 echo "Incorrect number of arguments"
  >&2 echo "Usage: $0 <voltage offset in mV>"
  exit 1
fi

undervolt --core "$1" --cache "$1"
