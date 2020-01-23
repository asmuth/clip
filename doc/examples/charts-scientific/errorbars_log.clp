(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(plot
  limit-y (0 1000)
  limit-x (0 1000)
  scale-y (logarithmic)
  errorbars (
      data-x (csv "test/testdata/errorbars.csv" "x")
      data-y-low (csv "test/testdata/errorbars.csv" "y_min")
      data-y-high (csv "test/testdata/errorbars.csv" "y_max"))
  points (
      data-x (csv "test/testdata/errorbars.csv" "x")
      data-y (csv "test/testdata/errorbars.csv" "y"))
  legend-overlay (
      item (label "Random Data")))
