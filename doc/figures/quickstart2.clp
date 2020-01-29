(layout/margins margin 4em)

(plot/axes
    limit-y (0 3)
    limit-x (0 1000)
    label-format-x (scientific)
    label-placement-x (linear-interval 100 100 900))

(plot/lines
    data-x (100 200 300 400 500 600 700 800 900)
    data-y (1.2   1.8   1.3   1.6   1.5   1.3   1.8   1.9   2.0)
    limit-y (0 3)
    limit-x (0 1000)
    marker-shape (pentagon)
    marker-size 8pt)

