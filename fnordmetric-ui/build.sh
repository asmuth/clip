#!/bin/bash

(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat js/d3.fnordmetric.js
  cat js/fnordmetric.js
  cat js/fnordmetric.rickshaw.js
  cat js/fnordmetric.util.js
  cat js/fnordmetric.widgets.counter.js
) > fnordmetric-ui.js

(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat css/fnordmetric.graph.css
) > fnordmetric-ui.css
