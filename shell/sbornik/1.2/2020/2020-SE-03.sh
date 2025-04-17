#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Script takes 2 arguments." >&2
    exit 1
fi

if [[ ! -d "$1" || ! -d "$2" ]]; then
    echo "Both arguments should be existing dirs."
    exit 2
fi

version=$(cat "${2}/version")

package=$(basename "$2")
package_name_in_repo="${package}-${version}"

temp_dir=$(mktemp -d)

archive="${temp_dir}/temp.tar.xz"

tar --xz -cf "${archive}" "${2}/tree"

checksum=$(sha256sum "${archive}" | cut -d ' ' -f1)

db="${1}/db"

field="${package}-${version}"

line=$(grep -E "^${field}" "${db}")
if [[ -z "${line}" ]]; then
    echo "${field} ${checksum}" >> "${db}"
    mv "${archive}" "$2/packages/${checksum}.tar.xz"
    sort "${db}" > "${db}"
else
    old_archive=$(echo "${line}" | cut -d ' ' -f2)
    rm "${1}/packages/${old_archive}.tar.xz"
    sed -i "s/^${field}.*/${field} ${checksum}" "${db}/"
    mv "${archive}" "$2/packages/${checksum}.tar.xz"

fi

rm -r "${temp_dir}"

