#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Error: The script requires exactly two numeric parameters."
    echo "Usage: $0 <number1> <number2>"
    exit 1
fi

if ! [[ "$1" =~ ^[0-9]+$ ]]; then
    echo "Error: The first parameter must be a positive number."
    exit 1
fi

if ! [[ "$2" =~ ^[0-9]+$ ]]; then
    echo "Error: The second parameter must be a positive number."
    exit 1
fi

if [ "$1" -ge "$2" ]; then
    echo "Error: The first parameter must be less than the second parameter."
    exit 1
fi


min_lines="$1"
max_lines="$2"

mkdir a b c

for file in $(find . -maxdepth 1 -type f); do

    lines=$(wc -l "${file}" | cut -d ' ' -f1)

    if [[ "${lines}" -le ${min_lines} ]]; then
        mv "${file}" a
        continue
    fi

    if [[ "${lines}" -le ${max_lines} ]]; then
        mv "${file}" b
        continue
    fi

    mv "${file}" c

done