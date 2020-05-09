(layer/resize 1200px 480px)
(layer/set-dpi 96)

(tools/plotgen
    limit-x (0 150)
    limit-y (1404278100 1404299700)
    axes (
        position (left bottom)
        label-format-y (datetime "%H:%M:%S"))
    areas (
        data-x (csv "test/testdata/measurement.csv" value2)
        data-y (csv "test/testdata/measurement.csv" time)
        direction horizontal
        color #999))
