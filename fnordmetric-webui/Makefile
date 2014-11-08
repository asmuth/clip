# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).

all: build

clean:
	rm fnordmetric-webui.js

build:
	( \
	  echo "/* THIS IS AN AUTOMATICALLY GENEREATED FILE --- DO NOT EDIT */"; \
	  cat fnordmetric-webui-layout.js \
	      fnordmetric-webui-util.js \
	      fnordmetric-webui-tableview.js \
	      fnordmetric-webui-metriclist.js \
	      fnordmetric-webui-queryeditor.js \
	      fnordmetric-webui-sqleditorview.js \
	      fnordmetric-webui-visualeditorview.js \
	      codemirror.js \
	) > fnordmetric-webui.js

watch:
	-while sleep 1; do fswatch -1 * && make; done

.PHONY: all clean build wtach
