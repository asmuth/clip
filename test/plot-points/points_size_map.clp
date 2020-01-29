(plot/draw-points
    data-x (csv test/testdata/gauss3d.csv x)
    data-y (csv test/testdata/gauss3d.csv y)
    sizes (csv test/testdata/gauss3d.csv z)
    size-map (linear 2pt 6pt)
    limit-x (0 400)
    limit-y (0 200))
