(set-width 2048px)
(set-height 768px)
(set-dpi 240)
(default font "Latin Modern Roman")

(default limit-x (0 60))
(default limit-y (-40 60))

(plot/add-axes)

(plot/draw-grid
    color (rgba 0 0 0 .05)
    stroke-style (dashed)
    tick-placement-x (none))

(plot/draw-points
    data-x (csv test/testdata/point_example.csv x)
    data-y (csv test/testdata/point_example.csv y)
    shape (circle-bullet)
    sizes (csv test/testdata/point_example.csv z)
    size-map (linear .4em 1.4em)
    labels (csv test/testdata/point_example.csv z))
