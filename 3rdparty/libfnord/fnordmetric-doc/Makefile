# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: build

build:
	find examples -name "*.sql" | while read file; do fnordmetric-cli -f svg -o $${file/.sql/.svg}.html $$file; done
	(cd web && rake build)

web: build
	(cd web/target && python -m SimpleHTTPServer 9090)

.PHONY: all clean build web
