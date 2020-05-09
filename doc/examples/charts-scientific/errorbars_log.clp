(layer/resize 1600px 900px)
(layer/set-dpi 240)
;;(layer/set-font "Latin Modern Roman")

(tools/plotgen
    limit-y (0 1000)
    limit-x (0 1000)
    scale-y (logarithmic)
    axes ()
    errorbars (
        data-x (csv "test/testdata/errorbars.csv" "x")
        data-y-low (csv "test/testdata/errorbars.csv" "y_min")
        data-y-high (csv "test/testdata/errorbars.csv" "y_max"))
    points (
        data-x (csv "test/testdata/errorbars.csv" "x")
        data-y (csv "test/testdata/errorbars.csv" "y"))
    legend (
        item (label "Random Data")))
