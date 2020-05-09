(layer/resize 2048px 1024px)
(layer/set-dpi 240)

(tools/plotgen
    limit-x (0 400)
    limit-y (0 200)
    axes (
        position (bottom left)
        label-placement-y (subdivide 4))
    points (
        data-x (csv test/testdata/gauss2d.csv x)
        data-y (csv test/testdata/gauss2d.csv y)))
