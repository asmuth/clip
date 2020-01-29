(plot/draw-points
    data-x (csv test/testdata/point_example.csv x)
    data-y (csv test/testdata/point_example.csv y)
    scale-x-padding 10
    scale-y-padding 20
    sizes (csv test/testdata/point_example.csv z)
    shape (circle-o 0.5))
