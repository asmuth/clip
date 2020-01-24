(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(default font "Latin Modern Roman")
(default limit-x (0 400))
(default limit-y (0 200))

(layout/add-margins margin 1em)

(plot/add-axes
    axis-y-label-placement (subdivide 5))

(plot/draw-points
    data-x (csv test/testdata/gauss3d.csv x)
    data-y (csv test/testdata/gauss3d.csv y)
    data-color (csv test/testdata/gauss3d.csv z)
    marker-shape (circle-o)
    color-map (gradient (0 #aaa) (1.0 #000)))

(figure/draw-legend
    position (bottom left)
    item (label "Random Data" marker-shape (circle-o)))
