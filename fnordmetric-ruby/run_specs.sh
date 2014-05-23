#!/bin/bash

(cd $(dirname $0)/../fnordmetric-ui && \
  ./build.sh)

(cd $(dirname $0)/web && \
  ./build.sh)

(cd $(dirname $0) && \
  bundle install && \
  bundle exec rspec)
