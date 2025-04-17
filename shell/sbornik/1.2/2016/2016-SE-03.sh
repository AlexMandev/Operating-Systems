#!/bin/bash

if [[ "${USER}" != "root" ]]; then
   echo "This script must be executed as root"
  exit 1
fi

if [[ $# -ne 0 ]]; then
    echo "This script takes no arguments"
    exit 2
fi

for user in $(cut -d':' -f1 /etc/passwd); do

    home_dir=$(grep -E "^${user}:" /etc/passwd | cut -d : -f6)
    user_info=$(grep -E "^${user}:" /etc/passwd)

    if [[ ! -d "${home_dir}" ]]; then
        echo "${user_info}"
        continue
    fi


    if [[ ! -w "${home_dir}" ]]; then
        echo "${user_info}"
    fi


done
