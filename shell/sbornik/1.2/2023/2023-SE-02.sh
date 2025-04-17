#!/bin/bash

if [[ $# -lt 2 ]]; then
    echo "At least 2 arguments." >&2
    exit 1
fi

if [[ ! "$1" =~ ^[1-9]+[0-9]*$ ]]; then
    echo "First argument should be a number" >&2
    exit 2
fi

sec="$1"
shift 1
command="${@}"

exec=0
all_sec=0

while [[ $(echo "${all_sec}") -lt "${sec}" ]]; do
    start=$(date +%s)
    $command &>/dev/null
    end=$(date +%s)
    exec=$((exec+1))
    all_sec=$(echo "$end - $start + $all_sec" | bc)
done
echo "Command was ran ${exec} times for ${all_sec} seconds."
echo "${exec} ${all_sec}" | awk '{print ($2 / $1)}'
