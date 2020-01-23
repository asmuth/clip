(chart/scatterplot
    data-x (csv test/testdata/gauss3d.csv x)
    data-y (csv test/testdata/gauss3d.csv y)
    data-size (csv test/testdata/gauss3d.csv z)
    size-map (linear 2pt 6pt)
    marker-shape (circle)
    limit-x (0 400)
    limit-y (0 200)
    axis-y-label-placement (subdivide 5)
    axes (bottom left)
    grid (color #fff)
    background #eee
    border none)
