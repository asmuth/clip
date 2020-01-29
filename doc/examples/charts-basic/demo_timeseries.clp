(width 2048px)
(height 512px)
(dpi 180)

(default limit-x (1404278100 1404299700))
(default limit-y (6000000 10000000))

(plot/axes
  position (bottom left)
  label-format-y (scientific)
  label-format-x (datetime "%H:%M:%S")
  label-placement-x (linear-align 1800))

(plot/lines
  data-x (csv "test/testdata/measurement.csv" time)
  data-y (csv "test/testdata/measurement.csv" value1)
  color #06c)

(figure/legend
    position (top right)
    item (label "Random Data" color #06c))
