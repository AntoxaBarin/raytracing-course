#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: ./run.sh <path-to-scene> <path-to-image>"
    exit 1
fi

./build/engine "$1" "$2"
