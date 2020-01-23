(chart/scatterplot
    data-x (csv test/testdata/point_example.csv x)
    data-y (csv test/testdata/point_example.csv y)
    data-size (csv test/testdata/point_example.csv z)
    labels (csv test/testdata/point_example.csv z)
    scale-x-padding 15
    scale-y-padding 15
    axes (bottom left)
    grid (color #fff)
    legend-overlay (item (label "Random Data"))
    background #eee
    border none)
