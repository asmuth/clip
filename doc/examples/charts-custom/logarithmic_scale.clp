(set height 300px)

(chart/linechart
    data-x (csv "test/testdata/log_example.csv" x)
    data-y (csv "test/testdata/log_example.csv" y)
    limit-x (1 5)
    limit-y (0 1000)
    scale-y (log)
    axes (bottom left)
    axis-y-label-format (base 10)
    grid (color #fff)
    background #eee
    border none)
