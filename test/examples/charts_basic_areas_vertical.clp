(set-height 480px)
(set-width 1200px)

(layout/add-margins margin 2em)

(default limit-x (0 150))
(default limit-y (1404278100 1404299700))

(plot/add-axes
    position  (left bottom)
    label-format-y (datetime "%H:%M:%S"))

(plot/draw-areas
    data-x (csv "test/testdata/measurement.csv" value2)
    data-y (csv "test/testdata/measurement.csv" time)
    direction horizontal
    color #999)
