(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(default limit-x (0 400))
(default limit-y (0 200))

(plot/add-axes
    position (bottom left)
    label-placement-y (subdivide 4))

(plot/draw-points
    data-x (csv test/testdata/gauss2d.csv x)
    data-y (csv test/testdata/gauss2d.csv y))
