#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

words=$(mktemp)
files=$(mktemp)

find "$1" -mindepth 1 -type f 2>/dev/null > "${files}"
files_count=$(cat "${files}" | wc -l)
f=$(echo "${files_count}/2" | bc)

while read file; do
    while read line; do
        word=$(echo "${line}" | awk '{ print $2 }')

        if ! grep -q -E " ${word} " "${words}"; then

            echo "${line}" >> "${words}"
        else
            ex_line=$(grep -E " ${word} " "${words}")
            occ=$(echo "${ex_line}" | awk '{ print $1} ')
            occ_file=$(echo "${ex_line}" | awk '{ print $3 }')

            to_add_occ=$(echo "${line}" | awk '{print $1}')

            if [[ "${to_add_occ}" -ge 3 ]]; then
                occ_file=$((occ_file + 1))
            fi

            new_occ=$(echo "${occ} + ${to_add_occ}" | bc)
            sed -i -E "s/^[0-9]+ ${word} [0-9]+/${new_occ} ${word} ${occ_file}/" "${words}"
        fi
    done < <(grep -Eo "\b[a-z]+\b" "${file}" | sort | uniq -c | awk '{if($1 >= 3) {print $0, 1} else {print $0, 0}}');
done < "${files}"

awk -v f="$f" '$3 >= f {print $1, $2}' "${words}" | sort -nr | head -n 10 | awk '{print $2}'


rm "${words}" "${files}"
