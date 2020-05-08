(width 2048px)
(height 1024px)
(dpi 240)

(font "Latin Modern Roman")
(limit-x (-500 500))
(limit-y (-200 200))

(axes)

(rectangles
    data-x (csv test/testdata/field2d.csv x)
    data-y (csv test/testdata/field2d.csv y)
    colors (csv test/testdata/field2d.csv a)
    color-map (gradient (0 #fff) (1.0 #000))
    size 10)
