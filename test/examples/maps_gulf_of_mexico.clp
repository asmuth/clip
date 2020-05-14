(layer/resize 2048px 1536px)
(layer/set-dpi 320)

(figure/plot
    limit-x (-100 -77)
    limit-y (16 32)
    axes (
        label-placement-x (linear 5)
        label-placement-y (linear 1)
        title-bottom "Longitude"
        title-left "Lattitude")
    background (fill #eff7fe)
    grid (
        tick-placement-x (linear 5)
        tick-placement-y (linear 5)
        color (rgba 0 0 0 0.1)
        stroke-style (dashed))
    polygons (
        data (geojson test/testdata/gulf_of_mexico.geojson)
        fill #ffefdb
        stroke-color #999187
        stroke-width .7pt)
    labels (
        data-x (-90)
        data-y (23.5)
        label-font-size 14pt
        label-color (rgba 0 0 0 0.6)
        labels ("Gulf of Mexico")))
