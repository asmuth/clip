(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    areas (
        data-x (csv "test/testdata/measurement.csv" time)
        data-y (csv "test/testdata/measurement.csv" value1)
        color #888))
