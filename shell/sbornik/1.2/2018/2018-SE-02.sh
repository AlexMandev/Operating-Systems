#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "This script takes 2 arguments."
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo "File should exist and be a regular file."
    exit 2
fi

if [[ ! -d "$2" ]]; then
    echo "$2: not a diretory"
    exit 3
fi

if [[ -n $(find "$2" -mindepth 1) ]]; then
    echo "$2: should be an empty directory."
    exit 4
fi

cat "$1" | grep -Eo "^[^\(:]+" | sed -E 's/[ ]+$//' | sort | uniq | awk '{print $0 ";" NR }' > "$2/dict.txt"

while read line; do
    name=$(echo "${line}" | cut -d ';' -f1)
    number=$(echo "${line}" | cut -d ';' -f2)

    cat "$1" | grep -E "^${name}[ ]*[\(:]" > "$2/${number}.txt"
done < <(cat "$2/dict.txt")
