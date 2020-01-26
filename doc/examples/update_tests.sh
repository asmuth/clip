#!/bin/bash
set -ue
cd "$(dirname "$0")/../.."

rm -f test/examples/*.clp

(cd doc/examples && find . -name "*.clp") | while read file; do
  cp \
    "doc/examples/${file}" \
    "test/examples/$(echo "${file}" | sed -e 's/^.\///' -e 's/-/_/g' -e 's/\//_/g')"
done
