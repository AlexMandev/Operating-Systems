#!/bin/bash

temp=$(mktemp)
counter=0
while true; do
    cms=$(ps -e -o rss=,cmd= | awk '{a[$2] += $1} END {for(cm in a) {if(a[cm] > 65536) {print cm}}}')

    if [[ -z "$cms" ]]; then
        break
    fi

    for command in $cms; do
        line=$(grep -E "^$command " "${temp}");
        if [[ -z "${line}" ]]; then
            echo "${command} 1" >> "${temp}"
            continue
        fi

        times=$(echo "${line}" | awk '{print $2}')
        sed -i -E "s/^${command} [0-9]+$/${command} $((times + 1))/" "${temp}"
    done

    sleep 1
    (( counter++ ))
done

limit=$(echo "${counter}/2" | bc)

while read line; do
   cnt=$(echo "${line}" | cut -d ' ' -f2)
   if [[ "$cnt" -ge "$limit" ]]; then
        echo "${line}" | awk '{ print $1 }'
   fi
done < "${temp}"

rm "${temp}"
