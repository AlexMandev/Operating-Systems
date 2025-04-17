#!/bin/bash

if [[ ${USER} != "root" ]]; then
    echo "This script must be run as root."
    exit 1
fi

if [[ $# -ne 1 ]]; then
    echo "Script takes one argument."
    exit 2
fi

info=$(ps -eo pid,ruser,rss | tail -n +2 | awk 'BEGIN { print "user,memory" }
                            { m[$2]+=$3 }
                            END { for(k in m) {print k "," m[k]}}')

echo "${info}"

while read line; do
    mem=$(echo "$line" | tail -n +2 | cut -d ',' -f2)

    if [[ "$mem" -gt "$1" ]]; then
        user=$(echo "$line" | cut -d ',' -f1)
        most_mem_pid=$(ps -eo rss,pid,ruser | awk -v user="$user" '$3 == user {print $0}' | sort -nr | head -n1 | awk '{print $2}')
        kill "$most_mem_pid"
    fi

done < <(echo "${info}")
