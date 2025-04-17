#!/bin/bash

# Debugging:
# set -euo pipefail

[[ "${#}" -eq 2 ]] || { echo "Need 2 arguments" >&2; exit 1; }
[[ -d "${1}" ]] || { echo "${1}: not a directory" >&2; exit 2; }
[[ ! -e "${2}" || -d "${2}" ]] || { echo "${2}: must not exist or must already be a directory" >&2; exit 3; }

CAMERA="${1}"
LIBRARY="${2}"

curr_files="$(mktemp)"
curr_start=""
curr_end=""

function cp_photos() {
	local to_filename
	local photo

	local to_dir="${LIBRARY}/${curr_start}_${curr_end}"

	mkdir -p "${to_dir}"

	while read to_filename photo; do
		cp "${photo}" "${to_dir}/${to_filename}.jpg"
	done < "${curr_files}"
}

while read time photo; do
	date="$(echo "${time}" | cut -d '_' -f 1)"
	to_filename="$(echo "${time}" | cut -d '.' -f 1)"

	if [[ -z "${curr_start}" ]]; then
		curr_start="${date}"
		curr_end="${date}"
	fi

	if [[ "${curr_end}" = "${date}" || "$(date -d "${curr_end} + 1 day" +'%Y-%m-%d')" = "${date}" ]]; then
		curr_end="${date}"
	else
		cp_photos

		echo -n > "${curr_files}"
		curr_start="${date}"
		curr_end="${date}"
	fi

	echo "${to_filename} ${photo}" >> "${curr_files}"
done < <(find "${CAMERA}" -type f -name '*.jpg' -printf '%TF_%TT %p\n' | sort -t ' ' -k 1,1)

cp_photos

rm "${curr_files}"