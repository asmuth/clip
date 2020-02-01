#!/bin/bash
set -ue
cd "$(dirname "$0")"

for f in $(find -name "*.gz"); do
  if [[ ! -e "${f/.gz/}" ]]; then
    gunzip --k "$f"
  fi
done
