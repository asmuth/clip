(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(layout/add-margins margin 1em)

(default limit-x (0 60))
(default limit-y (-40 60))

(plot/add-axes
    position (bottom left))

(plot/draw-points
    data-x (csv test/testdata/point_example.csv x)
    data-y (csv test/testdata/point_example.csv y)
    data-size (csv test/testdata/point_example.csv z)
    size-map (linear 1em 3em)
    labels (csv test/testdata/point_example.csv z)
    marker-shape (circle-bullet))
