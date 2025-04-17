#!/bin/bash

if [[ $# -eq 0 && $# -gt 2 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

temp_file=$(mktemp)
broken=0
for link in $(find "$1" -type l); do
   if [[ ! -e $(readlink "${link}") ]]; then
       (( broken++ ))
   else
        echo "$(basename "${link}") -> $(readlink "${link}")" >> "${temp_file}"
   fi

done

echo "Broken links: ${broken}" >> "${temp_file}"

if [[ -z "$2" ]]; then
    cat "${temp_file}"
else
    cat "${temp_file}" >> "$2"
fi

rm "${temp_file}"
