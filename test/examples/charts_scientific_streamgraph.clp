(set-width 2048px)
(set-height 768px)
(set-dpi 240)

(default font "Latin Modern Roman")
(default limit-y (-80 80))

(plot/add-axes)

(plot/draw-grid
    color (rgba 0 0 0 .05)
    stroke-style (dashed)
    tick-placement-y (none))

(plot/draw-areas
    data-x (csv "test/testdata/areadata2.csv" x)
    data-y (csv "test/testdata/areadata2.csv" z)
    data-y-low (csv "test/testdata/areadata2.csv" y)
    stroke-width 1pt
    fill (hatch angle 65 width .8pt stride 3.5pt))

(plot/draw-areas
    data-x (csv "test/testdata/areadata2.csv" x)
    data-y (csv "test/testdata/areadata2.csv" z2)
    data-y-low (csv "test/testdata/areadata2.csv" y2)
    stroke-width 1pt
    fill #eee)
