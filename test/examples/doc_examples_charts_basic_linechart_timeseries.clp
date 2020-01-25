(set-width 2048px)
(set-height 512px)
(set-dpi 240)

(layout/add-margins margin 1em)

(default limit-x (0 7200))
(default limit-y (-5 5))

(plot/add-axes
  position (bottom left)
  axis-y-label-format (scientific)
  axis-x-label-format (datetime "%H:%M:%S")
  axis-x-label-placement (linear-interval 900 900 7200))

(plot/draw-lines
  data-x (csv "test/testdata/measurement2.csv" time)
  data-y (csv "test/testdata/measurement2.csv" value3)
  color #06c)

(figure/draw-legend
    position (top right)
    item (label "Random Data" color #06c))
