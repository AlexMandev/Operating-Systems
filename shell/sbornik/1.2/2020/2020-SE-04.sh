#!/bin/bash

# find .jpg
# clean file names
# get title, album
# get date

[[ $# -eq 2 ]] || { echo "2 arguments"; exit 1; }

[[ -d "$1" ]] || { echo "$1: not an existing dir."; exit 2; }

[[ ! -e "$2" ]] || { echo "Second arg shouldn't exist as a directory"; exit 2; }

dst="$2"

mkdir -p "$dst/images"


while read entry; do
    file=$(echo "$entry" | cut -d ' ' -f2-)
    date=$(echo "$entry" | cut -d ' ' -f1)
    title=$(basename "$file" .jpg | sed -E "s/^[[:space:]]*//g" | sed -E "s/\([^\)]*\)//g" | sed -E "s/[[:space:]]+$//g" | tr -s ' ')

    album=$(basename "$file" .jpg | grep -Eo "\([^\)]+\)" | tail -n 1 | sed -E "s/[\(\)]//g")

    if [[ -z "$album" ]]; then
        album="misc"
    fi

    hash=$(sha256sum "$file" | cut -c 1-16)

    mkdir -p "$dst/by-date/${date}/by-album/${album}/by-title" 2>/dev/null
    mkdir -p "$dst/by-date/${date}/by-title" 2>/dev/null
    mkdir -p "$dst/by-album/${album}/by-date/${date}/by-title" 2>/dev/null
    mkdir -p "$dst/by-album/${album}/by-title" 2>/dev/null
    mkdir -p "$dst/by-title" 2>/dev/null

    cp "$file" "$dst/images/${hash}.jpg"
    absolute_path=$(realpath "$dst/images/${hash}.jpg")
    ln -s "$absolute_path" "$dst/by-date/${date}/by-album/${album}/by-title/${title}.jpg"
    ln -s "$absolute_path" "$dst/by-date/${date}/by-title/${title}.jpg"
    ln -s "$absolute_path" "$dst/by-album/${album}/by-date/${date}/by-title/${title}.jpg"
    ln -s "$absolute_path" "$dst/by-album/${album}/by-title/${title}.jpg"
    ln -s "$absolute_path" "$dst/by-title/${title}.jpg"

done < <(find "$1" -type f -name '*.jpg' -printf "%TF %p\n")
