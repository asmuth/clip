(set width 1000px)
(set height 400px)

(plot
    axes (left bottom)
    limit-x (0 150)
    axis-y-label-format (datetime "%H:%M:%S")
    areas (
        data-x (csv "tests/testdata/measurement.csv" value2)
        data-y (csv "tests/testdata/measurement.csv" time)
        direction horizontal
        color #999))
