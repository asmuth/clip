(layer/resize 1024px 60px)
(layer/set-dpi 96)

(figure/plot
    margin 1em
    axis (
        scale (logarithmic 10)
        tick-placement (exponential-steps 10 10)
        label-format (base 10)
        limit (0 10000)))
