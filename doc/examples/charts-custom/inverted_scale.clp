(width 2048px)
(height 800px)
(dpi 240)

(default scale-y (inverted))
(default limit-y (0 140))
(default limit-x (1404278100 1404299700))

(plot/axes
    position (bottom left)
    label-placement-x (subdivide 6)
    label-placement-y (linear-alignat 20 0)
    label-format-x (datetime "%H:%M:%S"))

(plot/lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2)
    color #06c)

