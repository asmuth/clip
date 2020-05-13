(layer/resize 2048px 768px)
(layer/set-dpi 240)
(layer/set-font "Latin Modern Roman")

(tools/plotgen
    limit-y (-80 80)
    axes (
        limit-x (0 1))
    grid (
        color (rgba 0 0 0 0.05)
        stroke-style (dashed)
        tick-placement-y (none))
    areas (
        data-x (csv "test/testdata/areadata2.csv" x)
        data-y (csv "test/testdata/areadata2.csv" z)
        data-y-low (csv "test/testdata/areadata2.csv" y)
        stroke-width 1pt
        fill (hatch angle 65 width .8pt stride 3.5pt))
    areas (
        data-x (csv "test/testdata/areadata2.csv" x)
        data-y (csv "test/testdata/areadata2.csv" z2)
        data-y-low (csv "test/testdata/areadata2.csv" y2)
        stroke-width 1pt
        fill #eee))
