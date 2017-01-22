#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root"
	exit 1
fi

gcc -o nom src/*.c -lm
cp nom /usr/bin/nom
rm nom
