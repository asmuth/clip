# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all:
	(cd build/cmake && make)

test:
	(cd build/cmake && make test)

clean:
	(cd build/cmake && make clean)

.PHONY: all test clean
