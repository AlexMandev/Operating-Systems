#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

if [[ ! -f "$1" ]]; then
    exit 2
fi

if [[ -z "$2" ]]; then
    exit 3
fi

s=$(awk -F ',' -v t="$2" '$5 == t {print $4}' "$1" | sort | uniq -c | sort -nr | head -n1 | sed -E "s/^[[:space:]]*//" | cut -d ' ' -f2-)

awk -F ',' -v s="$s" '$4 == s && $7 != "--" { print $0 }' "$1" | cut -d ',' -f1,7 | sort -t ',' -k2 -n | head -n1 | cut -d ',' -f '1'
