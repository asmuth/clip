(chart/linechart
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1)
    axes (bottom left)
    axis-x-title "Time"
    axis-y-title "Variable (f/s)"
    axis-y-label-format (scientific)
    axis-x-label-format (datetime "%H:%M:%S")
    grid (color #fff)
    background #eee
    border none)
