#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Script takes 3 arguments." >&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo "First argument file should exist" >&2
    exit 2
fi

if [[ -e "$2" ]]; then
    echo "Second argument file shouldn't exist." >&2
    exit 3
fi

if [[ ! -d "$3" ]]; then
    echo "${3}: directory doesn't exist." >&2
    exit 4
fi

users="$1"
config_file="$2"
dir="$3"

valid_files=$(mktemp)
while read file; do

    errors=$(grep -E -v -n "^#.*|\{ [a-zA-Z-]+;? \};|^[[:space:]]*$" "$file")

    if [[ -z "$errors" ]]; then
        echo "${file}" >> "$valid_files"
        continue
    fi

    echo "Errors in ${file}:"
    echo "${errors}"

done < <(find "$dir" -mindepth 1 -type f -name '*.cfg')

while read file; do
    cat "$file" >> "$config_file"
    user=$(basename "$file" .cfg)

    if [[ -z $(grep -E "^${user}:" "$users") ]]; then
        passwd=$(pwgen 16 1)
        echo "${user}:${passwd}"
        echo "${user}:${passwd}" >> "$users"
    fi

done < "${valid_files}"

rm "${valid_files}"
