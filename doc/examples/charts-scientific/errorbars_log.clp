(set-width 2048px)
(set-height 768px)
(set-dpi 240)

(default font "Latin Modern Roman")
(default limit-y (0 1000))
(default limit-x (0 1000))
(default scale-y (logarithmic))

(plot/add-axes)

(plot/draw-errorbars
    data-x (csv "test/testdata/errorbars.csv" "x")
    data-y-low (csv "test/testdata/errorbars.csv" "y_min")
    data-y-high (csv "test/testdata/errorbars.csv" "y_max"))

(plot/draw-points
    data-x (csv "test/testdata/errorbars.csv" "x")
    data-y (csv "test/testdata/errorbars.csv" "y"))

(figure/draw-legend
    item (label "Random Data"))
