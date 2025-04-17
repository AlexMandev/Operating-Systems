#!/bin/bash

if [[ "${USER}" != "oracle" && "${USER}" != "grid" ]]; then
    echo "User should be oracle or grid."
    exit 1
fi

if [[ -z "$ORACLE_HOME" ]]; then
    echo "Environmental variable ORACLE_HOME not set."
    exit 2
fi

adrci_path="${ORACLE_HOME}/bin/adrci"

if [[ ! -x "$adrci_path" ]]; then
    "Can't find the executable file ${adrci_path}"
    exit 3
fi

adrci_res=$("$adrci_path" exec="show homes")

if [[ "$adcri_res" == "No ADR homes are set." ]]; then
    exit 0
fi

adrci_res=$(echo "${adrci_res}" | tail -n +2)

cd "/u01/app/${USER}"

for rel_path in adrci_res; do
    absolute=$(realpath "${rel_path}")

    # ne sum siguren, kak se smqtashe razmer na direktoriq, poneje stat vrushtashe neshto drug
    # mislq che trqbva da stane s $du
done
