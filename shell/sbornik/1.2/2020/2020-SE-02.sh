#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Script takes 1 argument."
    exit 1
fi

if [[ ! -e "$1" ]]; then
    echo "Argument must be an existing file."
    exit 2
fi

top=$(awk '{print $2}' "$1" | sort | uniq -c | sort -nr | head -n 3 | awk '{print $2}' )

for site in $top; do
    line=$(awk -v site="${site}" '$2 == site { pr[$8]++ }
        END {
            second = 0
            first = 0
            for(key in pr) {
                if(key == "HTTP/2.0") {
                    second+=pr[key]
                }
                else {
                    first+=pr[key]
                }
            }
            print site, "HTTP/2.0:", second, "non-HTTP/2.0:", first
        }
    ' "$1")
    echo "${line}"

    awk -v site="${site}" '$2 == site && $9>302 {print $1}' "$1" | sort | uniq -c | sort -nr
done
