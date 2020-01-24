(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(chart/scatterplot
    data-x (csv test/testdata/point_example.csv x)
    data-y (csv test/testdata/point_example.csv y)
    data-size (csv test/testdata/point_example.csv z)
    size-map (linear 1em 3em)
    labels (csv test/testdata/point_example.csv z)
    limit-y (-40 60)
    marker-shape (circle-bullet)
    scale-x-padding 15
    scale-y-padding 15
    axes (bottom left)
    background #eee
    border none
    grid (color #fff))
