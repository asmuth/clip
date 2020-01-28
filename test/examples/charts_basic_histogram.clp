(set-width 1024px)
(set-height 512px)
(set-dpi 240)

(default limit-x (0 7))
(default limit-y (0 10000))
(default scale-y (log))

(plot/add-axes
    position (left bottom)
    label-placement-x (linear-interval 1 1 6))

(plot/draw-bars
    data-x (csv "test/testdata/histogram.csv" var0)
    data-y (csv "test/testdata/histogram.csv" var1)
    width (1.8em)
    color #666)
