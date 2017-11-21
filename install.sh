#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root"
	exit 1
fi

#gcc -o nom src/*.c -lm -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes
gcc -o nom src/*.c -lm -ggdb3 -O3 -w
cp nom /usr/bin/nom
rm nom
