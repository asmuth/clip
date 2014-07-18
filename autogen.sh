#!/bin/bash
if [[ "$1" == "clean" ]]; then
  rm -rf build/*

  find . \
    -name 'CMakeCache.txt' -o \
    -name 'CMakeFiles' -o \
    -name 'Makefile' -o \
    -name cmake_install.cmake -o \
    -name '*.so' -o \
    -name '*.dylib' -o \
    -name '*.bundle' -o \
    -name '*.dll' -o \
    -name '*.a' | \
    xargs rm -rf

  exit 0
fi

cd build && cmake . && make
