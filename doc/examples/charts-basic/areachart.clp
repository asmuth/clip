(limit-y (0 20000000))
(limit-x (1404278100 1404299700))

(axes
    position (bottom left)
    label-format-y (scientific)
    label-format-x (datetime "%H:%M:%S"))

(grid
    color #fff)

(areas
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1)
    color #888)
