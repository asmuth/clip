(set-width 2048px)
(set-height 800px)
(set-dpi 240)

(default scale-y (inverted))
(default limit-y (0 140))
(default limit-x (1404278100 1404299700))

(plot/add-axes
    position (bottom left)
    label-placement-x (subdivide 6)
    label-placement-y (linear-alignat 20 0)
    label-format-x (datetime "%H:%M:%S"))

(plot/draw-lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2)
    color #06c)

