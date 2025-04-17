#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

if [[ ! -f "$1" || ! -f "$2" ]]; then
    exit 2
fi

lines_first=$(grep -E "$1" "$1" | wc -l)
lines_second=$(grep -E "$2" "$2" | wc -l)

if [[ "${lines_first}" -gt "${lines_second}" ]]; then
    file="$1"
else
    file="$2"
fi

artist="${file}"
cat "${file}" | sed -E "s/[^\"]+//" | sort >> "${artist}.songs"
