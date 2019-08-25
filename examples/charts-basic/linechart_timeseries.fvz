(set width 2048px)
(set height 1024px)
(set dpi 240)

(chart/linechart
    data-x (csv "tests/testdata/measurement2.csv" time)
    data-y (csv "tests/testdata/measurement2.csv" value3)
    limit-y (-5 5)
    axes (bottom left)
    axis-x-label-format (datetime "%H:%M:%S")
    axis-x-label-placement (linear-interval 900 900 7200)
    grid (color #fff)
    background #eee
    border none
    legend-overlay (
        position (top right)
        border none
        margin .4em
        item (label "Random Data")))
