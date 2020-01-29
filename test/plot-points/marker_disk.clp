(plot/draw-points
    data-x (csv test/testdata/point_example.csv x)
    data-y (csv test/testdata/point_example.csv y)
    sizes (csv test/testdata/point_example.csv z)
    scale-x-padding 10
    scale-y-padding 20
    shape (circle))
