#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

#ln target target new_name

free_space=0
temp=$(mktemp)

while read file1; do
    hash=$(sha256sum "${file1}" | awk '{print $1}')
    size=$(wc -c "$file1" | awk '{print $1}')
    if grep -q -F "${hash}" "${temp}"; then
        continue
    fi

    deleted_files=0
    while read file2; do
        if [[ "$file1" == "$file2" ]]; then
            continue
        fi

        hash2=$(sha256sum "${file2}" | awk '{print $1}')

        if [[ $hash != $hash2 ]]; then
            continue
        else
            rm "$file2"
            free_space=$((free_space + size))
            ln "$file1" "$file2"

            if ! grep -q -E "^${hash}$" "${temp}"; then
                echo "${hash}" >> "${temp}"
            fi

        fi
    done < <(find "$1" -mindepth 1 -type f)
done < <(find "$1" -mindepth 1 -type f)

echo "Deleted groups of files: $(wc -l "${temp}" | awk '{print $1}')"
echo "Freed space: ${free_space}"

rm "${temp}"
