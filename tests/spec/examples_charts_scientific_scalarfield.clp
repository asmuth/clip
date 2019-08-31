(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(plot
  limit-x (-500 500)
  limit-y (-200 200)
  rectangles (
    data-x (csv tests/testdata/field2d.csv x)
    data-y (csv tests/testdata/field2d.csv y)
    data-color (csv tests/testdata/field2d.csv a)
    size 10
    color-map (gradient (0 #fff) (1.0 #000))))
