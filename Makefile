# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: build

build:
	(cd fnordmetric-core && make build)

install:
	(cd fnordmetric-core && make install)

test:
	(cd fnordmetric-core && make test)

clean:
	(cd fnordmetric-core && make clean)

doc:
	find doc/examples -name "*.sql" | while read file; do PATH=./build:$$PATH fnordmetric -f svg -o $${file/.sql/.svg}.html $$file; done
	(cd doc/web && rake build)

.PHONY: all test clean doc build
