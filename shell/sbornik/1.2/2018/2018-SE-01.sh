#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Expected 1 arg" >&2
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "${1}: not a directory."
fi

lines=$(mktemp)

while read dir; do
    friend=$(basename "${dir}")
    lines_with=0
    while read convo; do
        lines_with=$((lines_with + $(wc -l "$convo" | cut -d ' ' -f1)))
    done < <(find "$dir" -mindepth 1 -maxdepth 1 -type f -name '*.txt' | grep -E "[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}.txt$")

    search=$(grep -E "^${friend} " "${lines}")

    if [[ -z "${search}" ]]; then
        echo "${lines_with} ${friend}" >> "${lines}"
    else
        sed -i -E "s/^[0-9]+ ${friend}$/${lines_with} ${friend}/" "${lines}"
    fi

done < <(find "$1" -mindepth 3 -maxdepth 3 -type d)

cat "${lines}" | sort -nr | head -n 10

rm "${lines}"
