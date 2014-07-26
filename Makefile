# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all:
	mkdir -p build/tests
	(cd build && cmake .. && make)

test: all
	@find build/tests -iname "test-*" | while read t; do $$t || exit 1; done

clean:
	rm -rf build/*
	
	find . \
	    -name 'CMakeCache.txt' -o \
	    -name 'CMakeFiles' -o \
	    -name cmake_install.cmake -o \
	    -name '*.so' -o \
	    -name '*.dylib' -o \
	    -name '*.bundle' -o \
	    -name '*.dll' -o \
	    -name '*.a' | \
	    xargs rm -rf

.PHONY: all test clean
