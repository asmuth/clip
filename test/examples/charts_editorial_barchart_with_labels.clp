(height 280px)
(width 960px)

(default limit-x (0 13))
(default limit-y (0 23))

(plot/axes
    position (bottom)
    label-placement-x (linear-interval 1 1 12)
    label-placement-x (linear-interval 1 1 12)
    label-format-x (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec"))

(plot/bars
    data-x (csv "test/testdata/city_temperatures_london.csv" month)
    data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
    labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
    width (2em)
    color 2)

(figure/legend
    position (top left)
    border none
    item (label "London - Average Monthly Temperature" color 2))
