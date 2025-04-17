#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo "Script takes at least one argument"
    exit 250
fi

if [[ $# -gt 2 ]]; then
    echo "Too many arguments."
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "First argument should be a directory."
    exit 2
fi

if [[ $# -eq 2 ]]; then
    if [[ ! "$2" =~ ^[1-9]+[0-9]*$ ]]; then
        echo "Second argument should be a positive number"
        exit 3
    fi

    l="$2"
    ((l--))

    find "$1" -type f -links +"${l}"
else
    for link in $(find "$1" -type l); do
        file_name=$(readlink "${link}")

        if [[ ! -e "${file_name}" ]]; then
            echo "${link}"
        fi
    done
fi
