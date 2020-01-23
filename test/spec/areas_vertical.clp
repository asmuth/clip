(set width 600px)
(set height 900px)

(plot
    axes (left bottom)
    limit-x (0 150)
    axis-y-label-format (datetime "%H:%M:%S")
    areas (
        data-x (csv "test/testdata/measurement.csv" value2)
        data-y (csv "test/testdata/measurement.csv" time)
        direction horizontal
        color #999))
