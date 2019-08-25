(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(plot
    axes (top bottom)
    axis-left (
        limit (0 100)
        label-placement (linear-interval 10 10 50))
    axis-right (
        limit (0 1)
        label-placement (linear-interval 0.1 0.5 1.0))
    body (
      (plot/grid
          stroke-color #eee
          stroke-style dashed
          tick-placement-x (none))
      (plot/lines
          data-x (csv "tests/testdata/measurement2.csv" time)
          data-y (csv "tests/testdata/measurement2.csv" value3)
          limit-y (-4 10)
          stroke-width 0.8pt)
      (plot/lines
          data-x (csv "tests/testdata/measurement2.csv" time)
          data-y (csv "tests/testdata/measurement2.csv" value3)
          limit-y (-16 6)
          stroke-width 0.8pt)))
