#!/bin/bash

[[ $# -eq 2 ]] || { echo "2 arguments."; exit 1; }

[[ -f "$2" ]] || { echo "Second argument must be a file."; exit 2; }

domain="$1"
file="$2"

teams=$(awk '{ print $3 }' "$file" | sort | uniq)


for team in $teams; do
    echo "; team $team"

    zones=$(awk -v team="$team" '$3 == team {print $2}' "$file" | sort | uniq)
    servers=$(awk -v team="$team" '$3 == team {print $1}' "$file" | sort | uniq)

    for zone in $zones; do
        for server in $servers; do
            echo "${zone} IN NS ${server}.${domain}"
        done
    done
done
