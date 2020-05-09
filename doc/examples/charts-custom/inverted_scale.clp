(layer/resize 2048px 800px)
(layer/set-dpi 240)

(tools/plotgen
    scale-y (inverted)
    limit-y (0 140)
    limit-x (1404278100 1404299700)
    axes (
        position (bottom left)
        label-placement-x (subdivide 6)
        label-placement-y (linear-alignat 20 0)
        label-format-x (datetime "%H:%M:%S"))
    lines (
        data-x (csv "test/testdata/measurement.csv" time)
        data-y (csv "test/testdata/measurement.csv" value2)
        color #06c))
