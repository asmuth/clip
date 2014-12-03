# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all:
	mkdir -p target
	mkdir -p target/tests
	mkdir -p stage
	-test -e stage/fnord || ln -s ../../../ stage/fnord
	(cd target && cmake .. -DCMAKE_BUILD_TYPE=Release && make)

install:
	(cd target && cmake .. && make install)

test: all
	@find target/tests -iname "test-*" | while read t; do (cd ../../ && build/cmake/$$t) || exit 1; done

clean:
	rm -rf target stage

.PHONY: all test clean install
