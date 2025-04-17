#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "One argument"
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo "Not a regular file, or it doesn't exist"
    exit 2
fi


cat "$1" | sed -E "s/[0-9]{4} г\. - //" | awk '{print NR, $0}' | sort -t ' ' -k 2,2 | sed "s/ /. /"
