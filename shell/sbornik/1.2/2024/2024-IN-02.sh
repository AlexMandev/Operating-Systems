#!/bin/bash

dir="$1"
output_file="$2"

info=$(mktemp)

while read file; do

    declaration=$(grep -E "^class[[:space:]]+[a-zA-Z0-9_]+[[:space:]]*" "$file" | head -n 1)

    class_name=$(echo "${declaration}" | cut -d ':' -f1 | sed -E "s/^class[[:space:]]+//" | sed -E "s/[[:space:]]*$//" | sed -E "s/[^a-zA-Z0-9_]/ /g" | awk '{print $1}')

    echo "${class_name}" >> "$info"

    if [[ ! "$declaration" =~ .*:.* ]]; then
        continue
    fi

    bases=$(echo "$declaration" | cut -d ':' -f2 | sed -E "s/public|protected|private//g" | sed -E "s/[^a-zA-Z0-9_]/ /g")

    for base in $bases; do
        echo "${base}" >> "$info"
        echo "${base}->${class_name}" >> "$info"
    done

done < <(find "$dir" \( -name "*.cpp" -o -name "*.h" \) -type f)

cat "$info" | sort | uniq

#dag-ger "$info" >> "$output_file"

rm "$info"
