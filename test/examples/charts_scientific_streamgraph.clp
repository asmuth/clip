(width 1600px)
(height 900px)
(dpi 240)

(default font "Latin Modern Roman")
(default limit-y (-80 80))

(plot/axes)

(plot/grid
    color (rgba 0 0 0 .05)
    stroke-style (dashed)
    tick-placement-y (none))

(plot/areas
    data-x (csv "test/testdata/areadata2.csv" x)
    data-y (csv "test/testdata/areadata2.csv" z)
    data-y-low (csv "test/testdata/areadata2.csv" y)
    stroke-width 1pt
    fill (hatch angle 65 width .8pt stride 3.5pt))

(plot/areas
    data-x (csv "test/testdata/areadata2.csv" x)
    data-y (csv "test/testdata/areadata2.csv" z2)
    data-y-low (csv "test/testdata/areadata2.csv" y2)
    stroke-width 1pt
    fill #eee)
