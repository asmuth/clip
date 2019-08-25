(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(set margin-top 4em)
(set margin-bottom 4em)

(plot
    axes (right bottom left)
    scale-x (categorical (A B C D E F))
    limit-y (-10 70)
    axis-y-label-placement (linear-interval 10 0 60)
    bars (
      data-x (csv "tests/testdata/bardata.csv" var3)
      data-y (csv "tests/testdata/bardata.csv" var1)
      data-y-low (csv "tests/testdata/bardata.csv" var2)
      bar-width (1em)
      bar-offset (-.8em)
      bar-stroke-width .8pt
      bar-fill none)
    bars (
      data-x (csv "tests/testdata/bardata.csv" var3)
      data-y (csv "tests/testdata/bardata.csv" var4)
      data-y-low (csv "tests/testdata/bardata.csv" var5)
      bar-width (1em)
      bar-offset (.8em)
      bar-stroke-width .8pt
      bar-fill hatch)
    grid (
      stroke-color #000
      stroke-style dashed
      tick-placement-x (categorical-bounds)
      tick-placement-y (none))
    legend-top (
      position (top center)
      item-flow on
      margin 0
      padding-bottom 1em
      item (label "Series A" marker-shape (custom "▢"))
      item (label "Series B" marker-shape (square-hatch-diag-right)))
    axis-top (
      label-placement (none))
    margin-top 0)
