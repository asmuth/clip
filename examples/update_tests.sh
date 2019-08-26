#!/bin/bash
set -ue
cd "$(dirname "$0")/.."

rm tests/spec/examples_*.clp

find examples -name "*.clp" | while read file; do
  cp \
    "${file}" \
    "tests/spec/$(echo "${file}" | sed -e 's/^.\///' -e 's/-/_/g' -e 's/\//_/g')"
done
