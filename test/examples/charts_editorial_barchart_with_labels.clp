(set-height 280px)
(set-width 960px)

(default limit-x (0 13))
(default limit-y (0 23))

(layout/add-margins margin 1em)

(plot/add-axes
    position (bottom)
    axis-x-label-placement (linear-interval 1 1 12)
    axis-x-label-placement (linear-interval 1 1 12)
    axis-x-label-format (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec"))

(plot/draw-bars
    data-x (csv "test/testdata/city_temperatures_london.csv" month)
    data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
    labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
    bar-width (2em)
    color 2)

(figure/draw-legend
    position (top left)
    border none
    item (label "London - Average Monthly Temperature" color 2))
