(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    points (
        data (geojson "test/plot-points/points_geojson_data.geojson")
        size 8px))
