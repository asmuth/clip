(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat js/d3.fnordmetric.js
  cat js/fnordmetric.js
  cat js/fnordmetric.rickshaw.js
  cat js/fnordmetric.util.js
  cat js/fnordmetric.widgets.counter.js
) > fnordmetric-ui.js

cp fnordmetric-ui.js ../fnordmetric-core/web/fnordmetric-ui.js
