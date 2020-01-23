(set height 480px)
(set width 1200px)

(chart/linechart
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2)
    color #06c
    scale-y (inverted)
    limit-y (0 140)
    axes (bottom left)
    axis-x-label-placement (subdivide 6)
    axis-y-label-placement (linear-alignat 20 0)
    axis-x-label-format (datetime "%H:%M:%S"))
