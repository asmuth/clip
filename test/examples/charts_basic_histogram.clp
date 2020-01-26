(set-width 1024px)
(set-height 512px)
(set-dpi 240)

(layout/add-margins margin 1em)

(default limit-x (0 7))
(default limit-y (0 10000))
(default scale-y (log))

(plot/add-axes
    position (left bottom)
    axis-x-label-placement (linear-interval 1 1 6))

(plot/draw-bars
    data-x (csv "test/testdata/histogram.csv" var0)
    data-y (csv "test/testdata/histogram.csv" var1)
    bar-width (1.8em)
    color #666)
