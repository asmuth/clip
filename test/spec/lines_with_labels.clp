(set height 480px)
(set width 1200px)

(chart/linechart
    data-x (csv "test/testdata/city_temperatures_london.csv" month)
    data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
    labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
    scale-x-padding 0.6
    limit-y (-10 32)
    axes (bottom left)
    label-placement-x (linear-alignat 1 1)
    label-format-x (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    label-placement-y (linear-alignat 5 -10)
    color #06c
    marker-size 3pt
    legend-overlay (item (label "London" color #06c) position (bottom left)))
