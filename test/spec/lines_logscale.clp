(chart/linechart
    data-x (csv "test/testdata/log_example.csv" x)
    data-y (csv "test/testdata/log_example.csv" y)
    limit-x (1 5)
    limit-y (0 10000)
    scale-y (log)
    axis-y-label-placement (subdivide 10)
    grid (color #fff)
    background #eee
    border none)
