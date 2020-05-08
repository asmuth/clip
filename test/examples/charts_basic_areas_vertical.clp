(height 480px)
(width 1200px)

(limit-x (0 150))
(limit-y (1404278100 1404299700))

(axes
    position  (left bottom)
    label-format-y (datetime "%H:%M:%S"))

(areas
    data-x (csv "test/testdata/measurement.csv" value2)
    data-y (csv "test/testdata/measurement.csv" time)
    direction horizontal
    color #999)
