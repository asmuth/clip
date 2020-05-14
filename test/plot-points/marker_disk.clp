(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    points (
        data-x (csv test/testdata/point_example.csv x)
        data-y (csv test/testdata/point_example.csv y)
        scale-x-padding 10
        scale-y-padding 20
        shape (circle)))
