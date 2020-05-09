(layer/resize 1200px 480px)
(layer/set-dpi 96)

(tools/plotgen
    lines (
        data-x (csv "test/testdata/measurement.csv" time)
        data-y (csv "test/testdata/measurement.csv" value2)
        color #06c
        scale-y (inverted)
        limit-y (0 140)))
