(layer/resize 2048px 1024px)
(layer/set-dpi 240)
(layer/set-font "Latin Modern Roman")

(figure/plot
    limit-y (-4 5)
    limit-x (0 1000)
    points (
        data-x (100 200 300 400 500 600 700 800 900)
        data-y (-1.2 -1.8 -1.3 -1.6 -1.5 -1.3 -1.8 -1.9 -2.0)
        shape (pentagon)
        size 8pt)
    points (
        data-x (100 200 300 400 500 600 700 800 900)
        data-y (1.2 1.8 1.3 0.6 1.5 1.3 1.8 1.7 1.9)
        shape (square-bullet)
        size 8pt))
