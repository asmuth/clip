(plot
    limit-x (0 13)
    scale-y-padding 5
    axes (top right bottom left)
    axis-x-label-placement (linear-align 1)
    axis-x-label-placement (linear-align 1)
    axis-x-label-format (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    bars (
      data-x (csv "tests/testdata/city_temperatures.csv" month)
      data-y (csv "tests/testdata/city_temperatures.csv" temp_moscow)
      bar-width (2em)
      color #666))
