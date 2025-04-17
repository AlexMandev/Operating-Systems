#!/bin/bash

[[ "${USER}" == "root" ]] || { echo "This script must be run as root."; exit 1 }

[[ "$#" -eq 1 ]] || { echo "Script takes exactly one argument - a file name."; exit 2 }

[[ -f "$1" ]] || { echo "$1: file doesn't exist"; exit 3 }


config="$1"
files=$(mktemp)
while read line; do
   dir=$(echo "${line}" | awk '{ print $1 }')
   rule=$(echo "${line}" | awk '{ print $2 }')
   perms=$(echo "${line}" | awk '{ print $3 }')

   if [[ "$rule" == "R" ]]; then
       find "$dir" -mindepth 1 -type d,f -perm "$perms" -print0 > "$files"

   elif [[ "$rule" == "A" ]]; then
       find "$dir" -mindepth 1 -type d,f -perm /"$perms" -print0 > "$files"
   else
       # T
       find "$dir" -mindepth 1 -type d,f -perm -"$perms" -print0 > "$files"
   fi

   while read -d $'\0' file; do
       if [[ -f "$file" ]]; then
           chmod 664 "$file"
       else
           chmod 755 "$file"
       fi
   done < "$files"

done < "$config"

rm "$files"
