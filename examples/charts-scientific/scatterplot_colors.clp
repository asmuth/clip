(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(chart/scatterplot
    data-x (csv tests/testdata/gauss3d.csv x)
    data-y (csv tests/testdata/gauss3d.csv y)
    data-color (csv tests/testdata/gauss3d.csv z)
    limit-x (0 400)
    limit-y (0 200)
    marker-shape (circle-o)
    color-map (gradient (0 #aaa) (1.0 #000))
    axis-y-label-placement (subdivide 5)
    legend-overlay (
        position (bottom left)
        item (label "Random Data" marker-shape (circle-o))))
