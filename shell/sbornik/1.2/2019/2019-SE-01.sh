#!/bin/bash

nums=$(mktemp)

while read line; do
    echo "${line}" | grep -E "^-?([1-9]+[0-9]*|0)$" >> $nums
done

#a)
max=$(awk '{ if($0 ~ /-[0-9]*/) { num=-$0 } else { num = $0 } if(num >= max) {max = num} } END { print max }' "${nums}")

awk -v max="$max" '{ if ($0 ~ /-[0-9]*/) { num = -$0} else { num = $0} if(num == max) {print $0} }' "${nums}" | sort -n | uniq


#b)
digitNums=$(mktemp)
while read num; do
    digitSum="$(echo $num | grep -E -o "[0-9]" | awk '{sum+=$1} END {print sum}')"
    (echo "$num,$digitSum") >> $digitNums
done < $nums

max_sum=$(cat "${digitNums}" | sort -t ',' -k 2 -n | cut -d ',' -f 1 | head -1

rm "${nums}"
rm "${digitNums}"
