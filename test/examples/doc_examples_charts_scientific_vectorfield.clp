(set-width 2048px)
(set-height 1024px)
(set-dpi 240)
(default font "Latin Modern Roman")

(default limit-x (-4 4))
(default limit-y (-2 2))

(layout/add-margins margin 1em)

(plot/add-axes)

(plot/draw-vectors
    data-x (csv "test/testdata/vectorfield.csv" x)
    data-y (csv "test/testdata/vectorfield.csv" y)
    data-dx (csv "test/testdata/vectorfield.csv" dx)
    data-dy (csv "test/testdata/vectorfield.csv" dy))
