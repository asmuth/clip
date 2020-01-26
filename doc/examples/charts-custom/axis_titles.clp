(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(default limit-x (1404278100 1404299700))

(plot/add-axes
    label-format-left (scientific)
    title-left "Value"
    title-font-size-left 14pt
    label-format-right (scientific)
    title-right "Value"
    title-rotate-right 90
    title-font-size-right 14pt
    label-format-top (datetime "%H:%M:%S")
    title-top "Time"
    title-font-size-top 14pt
    label-format-bottom (datetime "%H:%M:%S")
    title-bottom "Time"
    title-font-size-bottom 14pt)

(plot/draw-lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1))
