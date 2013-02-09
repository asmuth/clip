#!/bin/bash

cp ../../fnordmetric-ui/fnordmetric-ui.js ./
cp ../../fnordmetric-ui/fnordmetric-ui.css ./

(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat vendor/jquery-ui.min.js
  cat vendor/jquery.maskedinput.js
  cat vendor/jquery.combobox.js
  cat js/fnordmetric.js
  cat js/fnordmetric.util.js
  cat js/fnordmetric.ui.js
  cat js/fnordmetric.timeline_widget.js
  cat js/fnordmetric.numbers_widget.js
  cat js/fnordmetric.bars_widget.js
  cat js/fnordmetric.pie_widget.js
  cat js/fnordmetric.toplist_widget.js
  cat js/fnordmetric.html_widget.js
  cat js/fnordmetric.realtime_timeline_widget.js
  cat js/fnordmetric.timeseries_widget.js
  cat js/fnordmetric.overview_view.js
  cat js/fnordmetric.gauge_view.js
  cat js/fnordmetric.dashboard_view.js
  cat js/fnordmetric.session_view.js
  cat js/fnordmetric.gauge_explorer.js
) > fnordmetric-core.js

(
  echo "/* this is an automatically generated file, don't modify it... */"
  cat vendor/font-awesome/css/font-awesome.css
  cat css/fnordmetric.core.css
) > fnordmetric-core.css

