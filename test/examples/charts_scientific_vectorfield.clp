(size 2048px 1024px)
(dpi 240)
(font "Latin Modern Roman")
(limit-x (-4 4))
(limit-y (-2 2))

(axes)

(vectors
    size 1pt
    data-x (csv "test/testdata/vectorfield.csv" x)
    data-y (csv "test/testdata/vectorfield.csv" y)
    data-dx (csv "test/testdata/vectorfield.csv" dx)
    data-dy (csv "test/testdata/vectorfield.csv" dy))
