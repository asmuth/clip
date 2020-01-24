#!/bin/bash
set -ue
cd "$(dirname "$0")/../.."

rm -f test/examples/*.clp

find doc/examples -name "*.clp" | while read file; do
  cp \
    "${file}" \
    "test/examples/$(echo "${file}" | sed -e 's/^.\///' -e 's/-/_/g' -e 's/\//_/g')"
done
