(set dpi 384)
(set width 3600px)
(set height 1920px)
(set font "Comic Sans MS" Regular)

(chart/linechart
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1)
    axes (bottom left)
    axis-y-label-format (scientific)
    axis-x-label-format (datetime "%H:%M:%S")
    grid (color #fff)
    background #eee
    border none)
