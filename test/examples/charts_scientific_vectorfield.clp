(width 1600px)
(height 900px)
(dpi 240)

(default font "Latin Modern Roman")
(default limit-x (-4 4))
(default limit-y (-2 2))

(plot/axes)

(plot/vectors
    size .7pt
    data-x (csv "test/testdata/vectorfield.csv" x)
    data-y (csv "test/testdata/vectorfield.csv" y)
    data-dx (csv "test/testdata/vectorfield.csv" dx)
    data-dy (csv "test/testdata/vectorfield.csv" dy))
