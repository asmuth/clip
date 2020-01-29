(set-width 600px)
(set-height 900px)

(plot/draw-areas
    data-x (csv "test/testdata/measurement.csv" value2)
    data-y (csv "test/testdata/measurement.csv" time)
    direction horizontal
    color #999)