#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Script takes 2 arguments."
    exit 1
fi

if [[ -f "$1" ]]; then
    echo "$1 already exists."
    exit 2
fi

if [[ ! -d "$2" ]]; then
    echo "Second file should be an existing diretory."
    exit 3
fi

function get_value {
    echo "$1" | cut -d ':' -f2 | sed 's/^ *//' | sed 's/ *$//'
}

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" > "$1"

while read file; do
    hostname=$(basename "${file}" ".log")

    content=$(cat "${file}")
    phy=$(get_value "$(echo "${content}" | grep -E "^Maximum Physical Interfaces")")
    vlans=$(get_value "$(echo "${content}" | grep -E "^VLANs")")
    hosts=$(get_value "$(echo "${content}" | grep -E "^Inside Hosts")")
    failover=$(get_value "$(echo "${content}" | grep -E "^Failover")")
    vpn=$(get_value "$(echo "${content}" | grep -E "^VPN-3DES-AES")")
    peers=$(get_value "$(echo "${content}" | grep -E "^\*Total VPN Peers")")
    trunk_ports=$(get_value "$(echo "${content}" | grep -E "^VLAN Trunk Ports")")

    license_line=$(echo "${content}" | grep -E "This platform has")
    license=$(echo "${license_line}" | sed "s/This platform has //" | sed -E "s/^[ ]+//" | sed -E "s/^a|^an//" | sed -E "s/^[ ]+//" | sed -E "s/license.[ ]*$//" \
    | sed -E "s/[ ]+$//" )

    sn=$(echo "${content}" | grep -E "^Serial Number" | cut -d ':' -f2 | tr -d ' ')
    key=$(echo "${content}" | grep -E "^Running Activation Key" | cut -d ':' -f2 | tr -d ' ')

    echo "$hostname,$phy,$vlans,$hosts,$failover,$vpn,$peers,$trunk_ports,$license,$sn,$key" >> "$1"

done < <(find "$2" -mindepth 1 -type f -name "*.log")
