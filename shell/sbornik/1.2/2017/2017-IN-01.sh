#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Not enough arguments"
    exit 1
fi

if [[ ! -f "$1" ]]; then
    exit 2
fi

first_terms=$(grep -E "$2=" "$1" | cut -d '=' -f2)

for term in $(grep -E "^$3=" "$1" | cut -d '=' -f2); do
    if [[ ! ${first_terms} =~ "${term}" ]]; then
        result="${result} ${term}"
    fi
done

result=$(echo "${result}" | sed -E "s/^ //" | sed -E "s/ $//")

sed -i "s/^$3=.*$/$3=${result}/" "$1"
