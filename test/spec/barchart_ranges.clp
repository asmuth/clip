(plot
    limit-x (0.5 6.5)
    limit-y (-20 70)
    axes (top right bottom left)
    axis-y-label-placement (linear-align 10)
    bars (
      data-x (csv "test/testdata/bardata.csv" var0)
      data-y (csv "test/testdata/bardata.csv" var1)
      data-y-low (csv "test/testdata/bardata.csv" var2)
      bar-width (1em)
      bar-offset (-.8em)
      color #ccc)
    bars (
      data-x (csv "test/testdata/bardata.csv" var0)
      data-y (csv "test/testdata/bardata.csv" var4)
      data-y-low (csv "test/testdata/bardata.csv" var5)
      bar-width (1em)
      bar-offset (.8em)
      color #666))
