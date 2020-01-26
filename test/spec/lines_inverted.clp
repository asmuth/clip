(set height 480px)
(set width 1200px)

(chart/linechart
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2)
    color #06c
    scale-y (inverted)
    limit-y (0 140)
    axes (bottom left)
    label-placement-x (subdivide 6)
    label-placement-y (linear-alignat 20 0)
    label-format-x (datetime "%H:%M:%S"))
