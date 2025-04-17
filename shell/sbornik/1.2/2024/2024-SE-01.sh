#!/bin/bash


if [[ $# -eq 0 ]]; then
    echo "No arguments given." >&2
    exit 1
fi

replaces=$(mktemp)
files=$(mktemp)

for arg in "$@"; do
    if [[ "$arg" =~ ^-R.+$ ]]; then

        if [[ ! "$arg" =~ ^-R[a-z0-9]+=[a-z0-9]+$ ]]; then
            echo "Invalid arg -R for replacing a word."
            rm "$replaces" "$files"
            exit 1

        else
            echo "${arg}" | sed -E "s/^-R//" | tr '=' ' ' >> "$replaces"
        fi

    else
        # arg is a file
        if [[ ! -f "$arg" ]]; then
            echo "${arg}: file doesn't exist"
            rm "$replaces" "$files"
            exit 2
        else
            echo "$arg" >> "$files"
        fi
    fi
done

pw=$(pwgen 64 1)

while read file; do
    while read entry; do

        to_replace=$(echo "${entry}" | cut -d ' ' -f1)
        replacement=$(echo "${entry}" | cut -d ' ' -f2)

        sed -i -E "s/\b${to_replace}\b/${pw}${replacement}${pw}/g" "$file"

    done < "$replaces"

    sed -i -E "s/\b${pw}//g" "$file"
    sed -i -E "s/${pw}\b//g" "$file"

done < "$files"



rm "$replaces" "$files"
