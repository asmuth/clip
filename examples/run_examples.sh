#!/bin/bash
set -uex

cd "${0%/*}/.."

find examples -name "*.plot" | while read f; do
  plotfx --in "$f" --out "${f/.plot/.svg}"
done
