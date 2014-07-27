# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: build

build:
	(cd build/cmake && make)

test:
	(cd build/cmake && make test)

clean:
	(cd build/cmake && make clean)
	rm -rf build/test/tmp*

doc:
	find examples -name "*.sql" | while read file; do fnordmetric -f svg -o $${file/.sql/.svg}.html $$file; done
	(cd doc && rake build)

.PHONY: all test clean doc build
