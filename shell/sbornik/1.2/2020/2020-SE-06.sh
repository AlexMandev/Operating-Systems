#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Scripts takes 3 arguments" >&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
    exit 2
fi

file="$1"
key="$2"
value="$3"

timestamp=$(date)

if grep -qE "^[[:space:]]*${key}[[:space:]]*=" "${file}"; then

    temp=$(mktemp)

    while IFS= read line; do
        if [[ "${line}" =~ ^[[:space:]]*${key}[[:space:]]*= ]]; then
            echo "# ${line} # edited at ${timestamp} by ${USER}" >> "${temp}"

            echo "${key} = ${value} # added at ${timestamp} by ${USER}" >> "${temp}"
        else
            echo "${line}" >> "${temp}"
        fi
    done < <(cat "${file}")

    cat "${temp}" > "${file}"

    rm "${temp}"
else
    echo "${key} = ${value} # added at ${timestamp} by ${USER}" >> "${file}"
fi
