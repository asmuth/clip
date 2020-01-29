(width 2048px)
(height 1024px)
(dpi 240)

(default font "Latin Modern Roman")
(default limit-x (-500 500))
(default limit-y (-200 200))

(plot/axes)

(plot/rectangles
    data-x (csv test/testdata/field2d.csv x)
    data-y (csv test/testdata/field2d.csv y)
    colors (csv test/testdata/field2d.csv a)
    color-map (gradient (0 #fff) (1.0 #000))
    size 10)
