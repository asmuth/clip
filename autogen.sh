#!/bin/bash
if [[ "$1" == "clean" ]]; then
  make clean
  exit 0
fi

make build
