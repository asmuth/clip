(size 2048px 1024px)
(dpi 240)
(font "Latin Modern Roman")
(limit-y (-4 5))
(limit-x (0 1000))

(axes
    label-format-x (scientific)
    label-placement-x (linear-interval 100 100 900))

(grid
    stroke-color (rgba 0 0 0 0.2)
    stroke-style dashed
    tick-placement-y (none))

(lines
    data-x (100 200 300 400 500 600 700 800 900)
    data-y (-1.2 -1.8 -1.3 -1.6 -1.5 -1.3 -1.8 -1.9 -2.0)
    marker-shape (pentagon)
    marker-size 8pt)

(lines
    data-x (100 200 300 400 500 600 700 800 900)
    data-y (1.2 1.8 1.3 0.6 1.5 1.3 1.8 1.7 1.9)
    marker-shape (square-bullet)
    marker-size 8pt)

(legend
    background #fff
    item-flow on
    item (label "Series A" marker-shape (pentagon))
    item (label "Series B" marker-shape (square-bullet)))
