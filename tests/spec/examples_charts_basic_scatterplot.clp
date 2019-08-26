(set width 2048px)
(set height 1024px)
(set dpi 240)

(chart/scatterplot
    data-x (csv tests/testdata/gauss2d.csv x)
    data-y (csv tests/testdata/gauss2d.csv y)
    limit-x (0 400)
    limit-y (0 200)
    axis-y-label-placement (subdivide 4)
    axes (bottom left)
    grid (color #fff)
    background #eee
    border none)
