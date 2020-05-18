(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    lines (
        data (geojson "test/plot-lines/lines_geojson_data.geojson")
        color #06c
        stroke-width 2pt))
