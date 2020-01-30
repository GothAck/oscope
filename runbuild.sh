#!/bin/bash
set -e
cd "$(dirname "$0")"

if [[ "$1" == "clean" ]]; then
	set -x
	rm -rf build
	exit 0
fi

set -x
mkdir -p build
cd build
cmake -GNinja ..
ninja $1
