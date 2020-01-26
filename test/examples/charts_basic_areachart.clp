(layout/add-margins margin 2em)

(default limit-y (0 20000000))
(default limit-x (1404278100 1404299700))

(plot/add-axes
    position (bottom left)
    axis-y-label-format (scientific)
    axis-x-label-format (datetime "%H:%M:%S"))

(plot/draw-grid
    color #fff)

(plot/draw-areas
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1)
    color #888)
