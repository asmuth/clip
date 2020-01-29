(default limit-x (0.5 6.5))
(default limit-y (-20 70))

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var0)
    data-y (csv "test/testdata/bardata.csv" var1)
    data-y-low (csv "test/testdata/bardata.csv" var2)
    width (1em)
    offset (-.8em)
    color #ccc)

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var0)
    data-y (csv "test/testdata/bardata.csv" var4)
    data-y-low (csv "test/testdata/bardata.csv" var5)
    width (1em)
    offset (.8em)
    color #666)
