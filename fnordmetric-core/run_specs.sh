#!/bin/bash

cd $(dirname $0) && \
  bundle install && \
  bundle exec rspec
