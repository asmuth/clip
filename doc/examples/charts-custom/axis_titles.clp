(set-width 2048px)
(set-height 1024px)
(set-dpi 240)

(default limit-x (1404278100 1404299700))

(plot/add-axes
    axis-left-label-format (scientific)
    axis-left-title "Value"
    axis-left-title-font-size 14pt
    axis-right-label-format (scientific)
    axis-right-title "Value"
    axis-right-title-rotate 90
    axis-right-title-font-size 14pt
    axis-top-label-format (datetime "%H:%M:%S")
    axis-top-title "Time"
    axis-top-title-font-size 14pt
    axis-bottom-label-format (datetime "%H:%M:%S")
    axis-bottom-title "Time"
    axis-bottom-title-font-size 14pt)

(plot/draw-lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1))
