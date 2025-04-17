#!/bin/bash


[[ $# -eq 1 ]] || { echo "1 arg."; exit 1; }

[[ -d "$1" ]] || { echo "$1: not an existing dir."; exit 2; }


touch "hash_db"

mkdir "/extracted"

temp_dir=$(mktemp -d)
temp_hash=$(mktemp)


while read file; do
    curr_hash=$(sha256sum "$file")

    filename=$(basename "$file")

    name=$(echo "$filename" | cut -d '_' -f1)
    timestamp=$(echo "$filename" | cut -d '_' -f2 | sed -E "s/^report-//" | sed -E "s/\.tgz$//")

    if ! grep -qE "^$curr_hash$" "$hash_db"; then
        tar -xzf  "$file" -C "$temp_dir"

        meow_file=$(find "$temp_dir" -type f -name 'meow.txt' 2>/dev/null | head -n1 )

        if [[ -n "$meow_file" ]]; then
            cp "$meow_file" "/extracted/${name}_${timestamp}.txt"
        fi

        rm -rf $temp_dir/*

        grep -Ev "^[a-z0-9]{64} ${file}$" "$hash_db" > "$temp_hash"
        cat "$temp_hash" > "$hash_db"
        echo -n > "$temp_hash"
        echo "${curr_hash}" >> "$hash_db"
    fi

done < <(find "$1" -name '*_report-[0-9]*.tgz')

rm -rf "$temp_dir" "$temp_hash"
