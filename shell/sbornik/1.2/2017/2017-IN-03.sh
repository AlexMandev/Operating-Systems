#!/bin/bash

entry=$(find /home -type f -printf '%T@ %u %p\n' 2>/dev/null | sort -nr | head -n 1)
file=$(echo "${entry}" | cut -d ' ' -f3-)
user=$(echo "${entry}" | cut -d ' ' -f2)

echo "User is: ${user}"
echo "File is: ${file}"
