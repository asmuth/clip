(set-width 2048px)
(set-height 512px)
(set-dpi 240)

(default limit-x (0 7200))
(default limit-y (-5 5))

(plot/add-axes
  position (bottom left)
  label-format-y (scientific)
  label-format-x (datetime "%H:%M:%S")
  label-placement-x (linear-interval 900 900 7200))

(plot/draw-lines
  data-x (csv "test/testdata/measurement2.csv" time)
  data-y (csv "test/testdata/measurement2.csv" value3)
  color #06c)

(figure/draw-legend
    position (top right)
    item (label "Random Data" color #06c))
