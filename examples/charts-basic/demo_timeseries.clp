(set width 2048px)
(set height 512px)
(set dpi 180)
(set margin-top 1em)

(plot
  axes (bottom left)
  axis-y-label-format (scientific)
  axis-x-label-format (datetime "%H:%M:%S")
  axis-x-label-placement (linear-align 1800)
  lines (
    data-x (csv "tests/testdata/measurement.csv" time)
    data-y (csv "tests/testdata/measurement.csv" value1)
    color #06c)
  legend-overlay (
      position (top right)
      item (label "Random Data" color #06c)))
