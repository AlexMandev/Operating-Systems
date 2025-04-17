#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "2 arguments."
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo "First argument must exist and be a regular file."
    exit 2
fi

if [[ -f "$2" ]]; then
    echo "Dest file already exists."
    exit 3
fi


temp=$(mktemp)

while read line; do
    copies=$(cat "$1" | grep -E "${line}")

    if [[ $(echo "${copies}" | wc -l) -gt 1 ]]; then
        id=$(echo "${copies}" | cut -d ',' -f 1 | sort -n | head -n 1)
        echo "$id,${line}" >> "$2"
    else
        echo "${copies}" >> "$2"
    fi

done < <(cat "$1" | cut -d ',' -f 2- | awk '{ print "," $0 }' | sort | uniq)
