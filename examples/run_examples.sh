#!/bin/bash
set -uex

cd "${0%/*}/.."

find examples -name "*.ptx" | while read f; do
  plotfx --in "$f" --out "${f/.ptx/.svg}"
done
