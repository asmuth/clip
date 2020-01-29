(width 2048px)
(height 768px)
(dpi 240)

(default font "Latin Modern Roman")
(default limit-y (0 1000))
(default limit-x (0 1000))
(default scale-y (logarithmic))

(plot/axes)

(plot/errorbars
    data-x (csv "test/testdata/errorbars.csv" "x")
    data-y-low (csv "test/testdata/errorbars.csv" "y_min")
    data-y-high (csv "test/testdata/errorbars.csv" "y_max"))

(plot/points
    data-x (csv "test/testdata/errorbars.csv" "x")
    data-y (csv "test/testdata/errorbars.csv" "y"))

(figure/legend
    item (label "Random Data"))
