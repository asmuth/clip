(set-width 2048px)
(set-height 512px)
(set-dpi 240)

(layout/add-margins margin 1em)

(default scale-x (categorical (A B C D E F)))
(default limit-y (0 80))

(plot/add-axes
    axis-y-label-placement (linear-interval 10 10 70))

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var1)
    bar-width (1em)
    bar-offset (-.8em)
    color #444)

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var2)
    bar-width (1em)
    bar-offset (.8em)
    color #999)
