#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

if [[ ! -f "$1" ]]; then
    exit 2
fi

if [[ ! -d "$2" ]]; then
    exit 3
fi

stopwords="$1"
dir="$2"

temp=$(mktemp)

while read file; do

    while read stopword; do
        words=$(grep -Eio "\b${stopword}\b" "$file")
        if [[ -z "$words" ]]; then
            continue
        fi
        replace=$(echo "${stopword}" | tr 'a-zA-Z0-9_' '*')
        while read word; do

            sed -E -i "s/\b${word}\b/${replace}/g" "${file}"

        done < <(echo "$words")

    done < "$stopwords"

done < <(find "$dir" -mindepth 1 -type f -name '*.txt')
