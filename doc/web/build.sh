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

cp doc/web/documentation.css "${output_dir}"
cp doc/web/redirects.txt "${output_dir}/_redirects"
cp doc/web/favicon.ico "${output_dir}"
mkdir -p "${output_dir}/figures"
cp doc/figures/*.svg "${output_dir}/figures/"
./doc/web/build_api_reference.py
./doc/web/build_examples.py
./doc/web/build_pages.py
