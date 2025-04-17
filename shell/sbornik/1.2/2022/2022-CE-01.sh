#!/bin/bash

base="base.csv"
prefix="prefix.csv"

if [[ $# -ne 3 ]]; then
    exit 1
fi

if [[ ! "$1" =~ ^-?[1-9]+[0-9]*$ && ! "$1" =~ ^-?[1-9][0-9]*\.[0-9]*$ ]]; then
    echo "Not a number"
    exit 2
fi

format=$(tail "${base}" -n +2 | awk -F ',' -v sym="$3" '$2 == sym { print $3 ", " $1 }')
format=$(echo "(${format})")

by=$(tail "${prefix}" -n +2 | awk -F ',' -v sym="$2" '$2 == sym { print $3 }')

res=$(echo "$1 * ${by}" | bc)
echo "${res} $3 ${format}"
