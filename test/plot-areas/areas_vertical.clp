(width 600px)
(height 900px)

(plot/areas
    data-x (csv "test/testdata/measurement.csv" value2)
    data-y (csv "test/testdata/measurement.csv" time)
    direction horizontal
    color #999)