(width 2048px)
(height 1024px)
(dpi 240)

(default limit-x (0 400))
(default limit-y (0 200))

(plot/axes
    position (bottom left)
    label-placement-y (subdivide 4))

(plot/points
    data-x (csv test/testdata/gauss2d.csv x)
    data-y (csv test/testdata/gauss2d.csv y))
