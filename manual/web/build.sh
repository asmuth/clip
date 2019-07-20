#!/bin/bash
set -ue

if [[ ! $# -eq 1 ]]; then
  echo "usage: $0 <output_dir>" >&2
  exit 1
fi

export output_dir="$(readlink -f $1)"

if [[ ! -d "${output_dir}" ]]; then
  echo "error: directory does not exist: ${output_dir}" >&2
  exit 1
fi

cd "$(dirname "$0")/../.."

cp manual/web/documentation.css "${output_dir}"
cp manual/web/redirects.txt "${output_dir}/_redirects"
cp manual/web/favicon.ico "${output_dir}"
mkdir -p "${output_dir}/documentation/figures"
cp manual/figures/*.svg "${output_dir}/documentation/figures/"
./manual/web/build_api_reference.py
./manual/web/build_examples.py
./manual/web/build_pages.py
