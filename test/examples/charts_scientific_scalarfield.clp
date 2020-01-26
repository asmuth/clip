(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(default font "Latin Modern Roman")
(default limit-x (-500 500))
(default limit-y (-200 200))

(layout/add-margins margin 1em)

(plot/add-axes)

(plot/draw-rectangles
    data-x (csv test/testdata/field2d.csv x)
    data-y (csv test/testdata/field2d.csv y)
    data-color (csv test/testdata/field2d.csv a)
    size 10
    color-map (gradient (0 #fff) (1.0 #000)))
