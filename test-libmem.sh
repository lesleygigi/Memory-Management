#!/bin/bash
cd "$(dirname "$0")"

if ! [[ -x libmem.so ]]; then
    echo "libmem.so does not exist"
    exit 1
fi

./tests/run-tests.sh $*
