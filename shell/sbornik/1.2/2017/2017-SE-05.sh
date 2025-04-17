#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

find "$1" -mindepth 1 -maxdepth 1 -name "vmlinuz-*.*.*-$2" | grep -E "^$1/vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-$2$" | sort -Vr | head -n 1
