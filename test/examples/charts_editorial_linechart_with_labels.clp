(set-width 2048px)
(set-height 800px)
(set-dpi 240)

(default scale-x-padding 0.6)
(default limit-x (0.5 12.5))
(default limit-y (0 25))

(layout/add-margins
    margin 1em)

(plot/add-axes
    position (bottom)
    label-placement-x (linear-alignat 1 1)
    label-format-x (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    label-placement-y (linear-alignat 5 -10))

(plot/draw-lines
    data-x (csv "test/testdata/city_temperatures_london.csv" month)
    data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
    labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
    color 1
    marker-size 5pt)

(figure/draw-legend
    item (label "London - Average Temperature" color 1)
    item-flow on
    border none)
