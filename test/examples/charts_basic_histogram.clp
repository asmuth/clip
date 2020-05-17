(layer/resize 1024px 512px)
(layer/set-dpi 240)

(figure/plot
    limit-x (0 7)
    limit-y (0 10000)
    scale-y (log)
    axes (
        position (left bottom)
        label-placement-x (linear-interval 1 1 6))
    bars (
        data-x (csv "test/testdata/histogram.csv" var0)
        data-y (csv "test/testdata/histogram.csv" var1)
        width 1.8em
        color #666))
