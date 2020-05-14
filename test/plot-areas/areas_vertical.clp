(layer/resize 600px 900px)
(layer/set-dpi 96)

(figure/plot
    areas (
        data-x (csv "test/testdata/measurement.csv" value2)
        data-y (csv "test/testdata/measurement.csv" time)
        direction horizontal
        color #999))
