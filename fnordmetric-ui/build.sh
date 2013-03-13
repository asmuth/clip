#!/bin/bash

version=$(
  cat js/fnordmetric.js | \
    grep "var version" | \
    sed -e 's/[^"]*"\([0-9.]*\).*/\1/'  
)

echo "* Building FnordMetric UI v$version..."

(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat js/d3.fnordmetric.js
  cat js/fnordmetric.js
  cat js/fnordmetric.rickshaw.js
  cat js/fnordmetric.util.js
  cat js/fnordmetric.widgets.timeseries.js
  cat js/fnordmetric.widgets.counter.js
) > fnordmetric-ui.js

echo "* fnordmetric-ui.js"

(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat css/fnordmetric.graph.css
) > fnordmetric-ui.css

echo "* fnordmetric-ui.css"

tar c fnordmetric-ui.js fnordmetric-ui.css > \
  fnordmetric-ui-$version.tar

echo "* fnordmetric-ui-$version.tar"
