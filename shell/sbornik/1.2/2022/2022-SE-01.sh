#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Script takes 1 argument" >&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo "arg isn't a file" >&2
    exit 2
fi

procfile="wakeup"
temp=$(mktemp)

while read line; do
    line=$(echo "$line" | sed -E "s/(^[^#]*)#.*$/\1/g" | tr -s ' ')

    dev=$(echo "${line}" | awk '{ print $1 }' )
    len="${#dev}"

    if [[ $len -gt 4 ]]; then
        echo "${dev} - device name should be 4 chars max."
        rm "${temp}"
        exit 3
    fi

    if ! grep -E "^${dev}" "${procfile}"; then
        echo "${dev} doesn't exist in procfile"
        rm "${temp}"
        exit 4
    fi

    state=$(echo "${line}" | awk '{ print $2 }' | grep -E "^(disabled|enabled)$")
    if [[ -n "${state}" ]]; then
        echo "${line}" >> "${temp}"
    else
        echo "${state} - values can be only disabled or enabled"
        rm "${temp}"
        exit 5
    fi


done < "$1"


while read line; do
    f1=$(echo "${line}" | awk '{ print $1 }')
    f2=$(echo "${line}" | awk '{ print $2 }')
    current=$(grep -E "^${f1}\b" "${procfile}" | awk ' { print $3 } ' | tr -d '*')
    if [[ "${current}" != "${f2}" ]]; then
        sed -i -E "s/(^${f1}.*)${current}/\1${f2}/" "${procfile}"
    fi
done > "${temp}"

rm "${temp}"
