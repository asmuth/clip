# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: build

build:
	(cd ../fnordmetric-webui && make)
	(cd build/cmake && make)

devserver: build
	mkdir -p /tmp/fnordmetric-data
	DEV_ASSET_PATH=../ ./build/cmake/target/fnordmetric-server --datadir /tmp/fnordmetric-data --statsd_port 1337

test:
	(cd build/cmake && make test)

clean:
	(cd build/cmake && make clean)
	rm -rf build/test/tmp*

.PHONY: all test clean build devserver
