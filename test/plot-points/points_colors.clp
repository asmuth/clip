(plot/points
    data-x (csv test/testdata/gauss3d.csv x)
    data-y (csv test/testdata/gauss3d.csv y)
    limit-x (0 400)
    limit-y (0 200)
    colors (csv test/testdata/gauss3d.csv z)
    color-map (gradient (0 #f00) (1.0 #00f)))