# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: build

clean:
	-rm app.bundle.js app.bundle.css app.bundle.html

build:
	@( \
    (cd ../ && cat fnordmetric-webui/app_assets.lst | grep -E '.js' | xargs -n1 cat) > app.bundle.js && \
    (cd ../ && cat fnordmetric-webui/app_assets.lst | grep -E '.css' | xargs -n1 cat) > app.bundle.css && \
    (cd ../ && cat fnordmetric-webui/app_assets.lst | grep -E '.html' | xargs -n1 cat) > app.bundle.html && \
    echo "<script>" >> app.bundle.html && \
    cat app.bundle.js >> app.bundle.html && \
    echo "</script>" >> app.bundle.html && \
    echo "<style type='text/css'>" >> app.bundle.html && \
    cat app.bundle.css >> app.bundle.html && \
    echo "</style>" >> app.bundle.html \
  )

atch:
	-while sleep 1; do fswatch -1 * && make; done

.PHONY: all clean build wtach
