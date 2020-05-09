(layer/resize 2048px 512px)
(layer/set-dpi 180)

(tools/plotgen
    limit-x (1404278100 1404299700)
    limit-y (6000000 10000000)
    axes (
        position (bottom left)
        label-format-y (scientific)
        label-format-x (datetime "%H:%M:%S")
        label-placement-x (linear-align 1800))
    lines (
        data-x (csv "test/testdata/measurement.csv" time)
        data-y (csv "test/testdata/measurement.csv" value1)
        color #06c)
    legend (
        position (top right)
        item (label "Random Data" color #06c)))
