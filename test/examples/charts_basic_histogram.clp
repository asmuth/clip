(width 1024px)
(height 512px)
(dpi 240)

(limit-x (0 7))
(limit-y (0 10000))
(scale-y (log))

(axes
    position (left bottom)
    label-placement-x (linear-interval 1 1 6))

(bars
    data-x (csv "test/testdata/histogram.csv" var0)
    data-y (csv "test/testdata/histogram.csv" var1)
    width (1.8em)
    color #666)
