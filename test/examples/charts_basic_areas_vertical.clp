(height 480px)
(width 1200px)

(default limit-x (0 150))
(default limit-y (1404278100 1404299700))

(plot/axes
    position  (left bottom)
    label-format-y (datetime "%H:%M:%S"))

(plot/areas
    data-x (csv "test/testdata/measurement.csv" value2)
    data-y (csv "test/testdata/measurement.csv" time)
    direction horizontal
    color #999)
