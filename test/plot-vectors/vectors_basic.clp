(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    limit-x (0 1000)
    limit-y (0 400)
    vectors (
        data-x (100 500 700 600 300)
        data-y (200 300 200 300 100)
        data-dx (10 5 -30 -14 12)
        data-dy (10 9 10 8 5)))
