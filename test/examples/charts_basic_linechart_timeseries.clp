(width 2048px)
(height 512px)
(dpi 240)

(default limit-x (0 7200))
(default limit-y (-5 5))

(plot/axes
  position (bottom left)
  label-format-y (scientific)
  label-format-x (datetime "%H:%M:%S")
  label-placement-x (linear-interval 900 900 7200))

(plot/lines
  data-x (csv "test/testdata/measurement2.csv" time)
  data-y (csv "test/testdata/measurement2.csv" value3)
  color #06c)

(figure/legend
    position (top right)
    item (label "Random Data" color #06c))
