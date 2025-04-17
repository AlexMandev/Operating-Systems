#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Script takes only one argument."
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "Not a directory"
    exit 2
fi

while read -r link; do

    if [[ ! -e $(readlink "${link}") ]]; then
        echo "${link}"
    fi
done < <(find "$1" -type l)
