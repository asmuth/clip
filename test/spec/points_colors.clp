(chart/scatterplot
    data-x (csv test/testdata/gauss3d.csv x)
    data-y (csv test/testdata/gauss3d.csv y)
    data-color (csv test/testdata/gauss3d.csv z)
    limit-x (0 400)
    limit-y (0 200)
    color-map (gradient (0 #f00) (1.0 #00f))
    axis-y-label-placement (subdivide 5)
    axes (bottom left)
    grid (color #fff)
    background #eee
    border none)
