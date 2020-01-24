(set-width 2048px)
(set-height 512px)
(set-dpi 240)

(plot
    axes (right bottom left)
    scale-x (categorical (A B C D E F))
    limit-y (0 80)
    axis-y-label-placement (linear-interval 10 10 70)
    bars (
      data-x (csv "test/testdata/bardata.csv" var3)
      data-y (csv "test/testdata/bardata.csv" var1)
      bar-width (1em)
      bar-offset (-.8em)
      color #444)
    bars (
      data-x (csv "test/testdata/bardata.csv" var3)
      data-y (csv "test/testdata/bardata.csv" var2)
      bar-width (1em)
      bar-offset (.8em)
      color #999)
    border none
    background #eee
    grid (color #fff))
