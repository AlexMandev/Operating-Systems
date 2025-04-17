#!/bin/bash

if [[ "${USER}" != "root" ]]; then
    echo "This script must be run as root."
    exit 1
fi

if [[ $# -ne 1 ]]; then
    echo "Script takes exactly one argument."
    exit 2
fi

proc=$(ps -eo user,pid,%cpu,%mem,vsz,rss,tty,stat,time,command | tail -n +2)

temp=$(mktemp)

user_proc=$(echo "${proc}" | grep -E "^$1 "|wc -l)
users=$(echo "${proc}" | awk '{ print $1 }' | sort | uniq)

for user in $users; do
    number_proc=$(echo "${proc}" | grep -E "^${user}" | wc -l)
    echo "${user} ${number_proc}" >> "${temp}"
done

#a)
echo "Users with more processes than $1"
awk -v m="$user_proc" ' $2 > m {print $1}' "${temp}"

#b)
sec=$(echo "${proc}" | awk '{ print $9 }' | awk -F ':' ' { seconds += $3 + $2*60 + $1*3600 } END { seconds /= NR; print seconds} ')
echo -n "Average seconds for a process: "
echo "${sec}"

#c)
to_kill=$(echo "${proc}" | grep -E "^$1" | awk '{print $2 ":" $9 }' | awk -F ':' -v t="$sec" '{ if($4 + $3*60 + $2*3600 > 2*t) {print $1}}')

for pr in $to_kill; do
    #kill "${pr}"
    echo "Killing ${pr}"
done

rm "${temp}"