(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    points (
        data-x (csv test/testdata/gauss3d.csv x)
        data-y (csv test/testdata/gauss3d.csv y)
        limit-x (0 400)
        limit-y (0 200)
        colors (csv test/testdata/gauss3d.csv z)
        color-map (steps (0 #f00) (0.2 #0f0) (0.4 #00f) (0.6 #0f0) (0.8 #f00) (1.0 #00f))))
