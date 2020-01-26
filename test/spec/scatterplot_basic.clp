(chart/scatterplot
    data-x (csv test/testdata/gauss2d.csv x)
    data-y (csv test/testdata/gauss2d.csv y)
    axes (bottom left)
    label-placement-y (subdivide 5)
    grid (color #fff)
    legend-overlay (item (label "Random Data"))
    background #eee
    border none
    limit-x-min 0
    limit-x-max 400
    limit-y-min 0
    limit-y-max 200)
