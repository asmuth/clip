(set-width 2048px)
(set-height 1200px)
(set-dpi 240)

(default limit-x (1404278100 1404299700))
(default limit-y (0 1))

(plot/add-axes
    label-placement-left (subdivide 4)
    label-format-left (scientific)
    label-attach-left bottom
    label-rotate-left -90
    label-format-bottom (datetime "%H:%M:%S")
    label-attach-bottom left
    label-rotate-bottom 45)

(plot/draw-lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1))

