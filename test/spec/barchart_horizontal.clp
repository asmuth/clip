(plot
    limit-x (0 24)
    limit-y (0 10)
    axes (left)
    label-format-y (custom (csv "test/testdata/gdp_per_capita_2010.csv" country))
    bars (
      data-x (csv "test/testdata/gdp_per_capita_2010.csv" gdp)
      data-y (csv "test/testdata/gdp_per_capita_2010.csv" rank)
      labels (csv "test/testdata/gdp_per_capita_2010.csv" gdp_label)
      direction horizontal
      bar-width (1.2em)
      color #666))
