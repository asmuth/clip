(plot
  axis-bottom (
    limit (0 100))
  lines (
        data-x (csv "test/testdata/measurement.csv" time)
        data-y (csv "test/testdata/measurement.csv" value2)
        color #000))
