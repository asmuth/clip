# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: assets
	mkdir -p target/tests
	mkdir -p stage/src
	test -e stage/src/fnordmetric || ln -s ../../../../src stage/src/fnordmetric || true
	(cd target && cmake .. && make)

assets:
	mkdir -p stage/assets
	(cd ../.. && xxd -i assets/index.html) > stage/assets/asset_index.html.h
	(cd ../.. && xxd -i assets/fnordmetric-web.css) > stage/assets/fnordmetric_web.css.h

test: all
	@find target/tests -iname "test-*" | while read t; do (cd ../../ && build/cmake/$$t) || exit 1; done

clean:
	rm -rf target stage

.PHONY: all test clean
