# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: assets
	mkdir -p target/tests
	mkdir -p stage/src
	test -e stage/src/fnordmetric || ln -s ../../../../src stage/src/fnordmetric || true
	test -e stage/src/fnord || ln -s ../../../../../libfnord stage/src/fnord || true
	(cd target && cmake .. -DCMAKE_BUILD_TYPE=Release && make)

install:
	(cd target && cmake .. && make install)

assets:
	./assets.sh

test: all
	@find target/tests -iname "test-*" | while read t; do (cd ../../ && build/cmake/$$t) || exit 1; done

clean:
	rm -rf target stage

.PHONY: all test clean assets install
