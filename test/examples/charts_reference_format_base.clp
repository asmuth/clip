(set-height 80px)
(layout/add-margins margin 2em)

(plot/draw-axis
    scale (logarithmic 2)
    label-format (base 2)
    limit (0 4096))
