(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(plot
    limit-y (-80 80)
    areas (
        data-x (csv "tests/testdata/areadata2.csv" x)
        data-y (csv "tests/testdata/areadata2.csv" z)
        data-y-low (csv "tests/testdata/areadata2.csv" y)
        stroke-width 1pt
        fill (hatch angle 65 width .8pt stride 3.5pt))
    areas (
        data-x (csv "tests/testdata/areadata2.csv" x)
        data-y (csv "tests/testdata/areadata2.csv" z2)
        data-y-low (csv "tests/testdata/areadata2.csv" y2)
        stroke-width 1pt
        fill #eee))
