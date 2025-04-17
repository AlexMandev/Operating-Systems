#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Script takes exactly one argument." >&2
    exit 1
fi

if [[ ! "$1" =~ ^[1-9]+[0-9]*$ ]]; then
    echo "Argument must be a positive number." >&2;
    exit 2
fi

temp=$(mktemp)
echo "A Bb B C Db D Eb E F Gb G Ab" >> "${temp}"

N="$1"

if [[ "$N" -ge 12 ]]; then
    (( N %= 12 ))
fi

changes=$(mktemp)

awk -v n="$N" '{ for(i=1;i<=NF;i++) { change=(i+n)%12; if(change==0) {change=1} print $i, $change } }' "${temp}" >> "${changes}"

text=$(mktemp)

while read line; do
    echo "${line}" >> "${text}"
done

while read line; do
    new_line="$line"
    for chord in $(echo "${line}" | grep -Eo "\[[A-G][^\[]*\]"); do
        note=$(echo "${chord}" | grep -Eo "^\[[A-G]b?" | tr -d '[')
        changed_note=$(grep "^${note} " "${changes}" | awk '{ print $2 }')
        new_line=$(echo "${new_line}" | sed -E "s/\[${note}/\[0${changed_note}/")

    done
    echo "${new_line}" | sed -E "s/\[0/\[/g"
done < "${text}"

rm "${changes}" "${temp}" "${text}"
