(plot
    axes (bottom left)
    label-format-y (scientific)
    label-format-x (datetime "%H:%M:%S")
    areas (
      data-x (csv "test/testdata/measurement.csv" time)
      data-y (csv "test/testdata/measurement.csv" value1)
      color #888)
    grid (color #fff)
    background #eee
    border none)
