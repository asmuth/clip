(set width 2048px)
(set height 1024px)
(set dpi 240)
(set font "Latin Modern Roman")

(plot
  limit-x (-4 4)
  limit-y (-2 2)
  vectors (
      data-x (csv "test/testdata/vectorfield.csv" x)
      data-y (csv "test/testdata/vectorfield.csv" y)
      data-dx (csv "test/testdata/vectorfield.csv" dx)
      data-dy (csv "test/testdata/vectorfield.csv" dy)))
