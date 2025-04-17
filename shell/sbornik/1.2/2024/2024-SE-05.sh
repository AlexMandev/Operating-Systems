#!/bin/bash

[[ $# -eq 2 ]] || { echo "Script takes 2 arguments." >&2; exit 1; }

result=$("$1")

[[ "${?}" -eq 0 ]] || { exit 3; }

timestamp=$(date "+%Y-%m-%d %H:%M:%S")
day_of_week=$(date "+%u")
hour=$(date "+%H")

data="$2"

if [[ ! -e "$data" ]]; then
    # first execution, exiting with diff status
    echo "$timestamp $day_of_week $hour $result" >> "$data"
    exit 4
fi

echo "$timestamp $day_of_week $hour $result" >> "$data"

if [[ -f "$data" ]]; then
    other_results=$(awk -v day="$day_of_week" hour="$hour" '$3 == day && $4 == hour {print $5}' "$data")

    count=$(echo "$other_results" | wc -l)


    if [[ "$count" -gt 1 ]]; then
        avg=$(echo "$other_results" | head -n -1 | awk '{ sum += $1 } END { print sum/NR }')

        half_avg=$(echo "$avg / 2" | bc -l)
        double_avg=$(echo "$avg * 2" | bc -l)

        awk_res=$(echo "$result $half_avg $double_avg" | awk '$1 < $2 || $1 > $3 { print "yes"}')

        if [[ "$awk_res" == "yes" ]]; then
            echo "$(date "+%Y-%m-%d %H"): ${result} abnormal"
            exit 2
        fi

    fi
fi

exit 0
