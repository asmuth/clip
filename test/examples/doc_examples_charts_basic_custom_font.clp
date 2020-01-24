(set-width 2048px)
(set-height 512px)
(set-dpi 240)

(default font "Comic Sans MS")

(chart/linechart
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2)
    limit-y (0 120)
    axes (bottom left)
    axis-y-label-format (fixed 2)
    axis-x-label-format (datetime "%H:%M:%S")
    axis-y-label-placement (linear 20)
    grid (color #fff)
    background #eee
    border none
    legend-overlay (
        position (top right)
        border none
        margin 0.5em
        item-flow on
        item (
            label "A very serious measurement")))
