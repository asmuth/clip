(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    limit-x (0 1000)
    limit-y (0 400)
    areas (
        data-x (100 300 500 700 900)
        data-y (200 300 200 300 100)
        color #06c))
