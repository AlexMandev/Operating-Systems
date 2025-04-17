#!/bin/bash

lines=10

if [[ $# -eq 0 ]]; then
    echo "No arguments given."
    exit 1
fi

if [[ "$1" == "-n" ]]; then
    if [[ $# -eq 1 ]]; then
        echo "Args: [-n N] files..."
        exit 2
    fi

    if [[ "$2" =~ ^[1-9]+[0-9]*$ ]]; then
        echo "[-n N] N must be a positive number."
        exit 3
    fi
    lines="$2"
    shift 2
fi

temp=$(mktemp)

for file in "$@"; do
    file_name=$(basename "${file}" ".log")

    while read line; do
        date_time=$(echo "${line}" |cut -d ' ' -f1-2)
        text=$(echo "${line}" | cut -d ' ' -f3-)
        echo "${date_time} ${file_name} ${text}" >> "${temp}"
    done < <(tail "${file}" -n $lines)

done

cat "${temp}" | sort -t ' ' -k1,2

rm "${temp}"
