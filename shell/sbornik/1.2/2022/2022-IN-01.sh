#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

if [[ ! -d "$1" || ! -d "$2" ]]; then
    exit 2
fi

if [[ -n "$(find $2 -mindepth 1)" ]]; then
    exit 3
fi

abs_dest=$(realpath "$2")

cd "$1"

while read file; do

    if [[ "$(basename "${file}")" =~ ^\..*\.swp$ ]]; then
        continue
    fi

    cp --parents "$file" "${abs_dest}"
done < <(find . -type f)
