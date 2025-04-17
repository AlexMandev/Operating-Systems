#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Script accepts 3 parameters."
    exit 1
fi

if [[ ! -d "$1" || ! -d "$2" ]]; then
    echo "First two arguments should be directiories."
    exit 2
fi

if [[ -n "$(find "$2" -mindepth 1)" ]]; then
    echo "Second directory should be empty."
    exit 3
fi

if [[ "${USER}" != "root" ]]; then
    echo "This script must be run as root."
    exit 4
fi

while read file; do

    file_dir=$(dirname "${file}")

    rel_dir=$(realpath --relative-to="$1" "$file_dir")

    if [[ "${rel_dir}" == "." ]]; then
        target_dir="$2"
    else
        target_dir="$2/${rel_dir}"
    fi

    mkdir -p "${target_dir}" 2> /dev/null

    mv "${file}" "${target_dir}/"


done < <(find "$1" -type f -name "*$3*")
