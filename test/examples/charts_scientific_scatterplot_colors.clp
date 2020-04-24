(width 1600px)
(height 900px)
(dpi 240)

(default font "Latin Modern Roman")
(default limit-x (0 400))
(default limit-y (0 200))

(plot/axes
    label-placement-y (subdivide 5))

(plot/points
    data-x (csv test/testdata/gauss3d.csv x)
    data-y (csv test/testdata/gauss3d.csv y)
    shape (circle-o)
    colors (csv test/testdata/gauss3d.csv z)
    color-map (gradient (0 #aaa) (1.0 #000)))

(figure/legend
    position (bottom left)
    item (label "Random Data" marker-shape (circle-o)))
