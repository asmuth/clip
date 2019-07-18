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

cp extra/web/documentation.css "${output_dir}"
cp extra/web/redirects.txt "${output_dir}/_redirects"
cp extra/web/favicon.ico "${output_dir}"
mkdir -p "${output_dir}/documentation/figures"
cp manual/figures/*.svg "${output_dir}/documentation/figures/"
./extra/web/build_api_reference.py
./extra/web/build_examples.py
./extra/web/build_pages.py
