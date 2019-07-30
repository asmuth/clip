Color Maps
==========

The `<color-map>` setting allows you to map input data to colors. These are the
currently implemented mapping methods:

  - `discrete` -- Read integer values from the input file and map them to a discrete color palette
  - `linear` -- Read numeric values from the input file and linearly map them to a color gradient
  - `raw` -- Parse colors in hex format from the input file

#### Syntax:

    ;; a discrete color map
    (discrete (<color>...) <data-ref>)

    ;; a discrete color map using a default palette
    (discrete <color-palette-name> <data-ref>)

    ;; a linear color map
    (linear (<color> <color>) <data-ref>)

    ;; a linear "default" color map
    (linear <color-map-name> <data-ref>)

    ;; a "raw" color map
    (raw <data-ref>)


#### Examples:

    ;; map the values of 'mycolorcolumn' to {red, green, blue}.
    ;; values in 'mycolorcolumn' should be in the set {1, 2, 3}
    (discrete (#f00 #0f0 #00f) (csv "myfile.csv" "mycolorcolumn"))

    ;; map the values of 'mycolorcolumn' to [red..blue].
    ;; values in 'mycolorcolumn' should be in [0..1]
    (linear #f00 #00f (csv "myfile.csv" "mycolorcolumn"))

    ;; read hex color codes from 'mycolorcolumn'
    ;; values in 'mycolorcolumn' should be hex color codes (e.g. "#f00")
    (raw (csv "myfile.csv" "mycolorcolumn"))


List of built-in color maps
---------------------------

