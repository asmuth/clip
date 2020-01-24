(default limit-x (0.5 6.5)
(default limit-y (-20 70))

(plot/add-axes
    axis-y-label-placement (linear-align 10))

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var0)
    data-y (csv "test/testdata/bardata.csv" var1)
    data-y-low (csv "test/testdata/bardata.csv" var2)
    bar-width (1em)
    bar-offset (-.8em)
    color #ccc)

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var0)
    data-y (csv "test/testdata/bardata.csv" var4)
    data-y-low (csv "test/testdata/bardata.csv" var5)
    bar-width (1em)
    bar-offset (.8em)
    color #666)
