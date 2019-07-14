#!/bin/bash
set -uex

cd "${0%/*}/.."

find examples -name "*.fvz" | while read f; do
  fviz --in "$f" --out "${f/.fvz/.svg}"
done
