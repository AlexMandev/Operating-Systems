#!/bin/bash

local_users=$(mktemp)
cloud_users=$(mktemp)
awk -F ':' '$3 >= 1000 {print $0}' "/etc/passwd" | cut -d ':' -f1 >> "${local_users}"
path=$(realpath $(dirname "$0"))

"$path"/occ user:list | sed -E "s/^- //" | cut -d ':' -f1 >> "${cloud_users}"


while read user; do

    if ! grep -qE "^${user}$" "$cloud_users"; then
        #user not in cloud, adding...
        "$path"/occ user:add "${user}"
    else
        cloud_info=$("$path"/occ user:info "${user}")
        flag=$(echo "${cloud_info}" | sed -E "s/^- //" | grep -E "^enabled:" | awk '{ print $2 }')

        if [[ "$flag" == "false" ]]; then
            #unlocking...
            "$path"/occ user:enable "${user}"
        fi
    fi


done < "$local_users"

while read cloud_user; do
    if ! grep -qE "^${cloud_user}$" "$local_users"; then
        cloud_info=$("$path"/occ user:info "${cloud_user}")
        flag=$(echo "${cloud_info}" | sed -E "s/^- //" | grep -E "^enabled:" | awk '{ print $2 }')

        if [[ "$flag" == "true" ]]; then
            "$path"/occ user:disable "${cloud_user}"
        fi
    fi
done < "$cloud_users"

rm "$local_users" "$cloud_users"
