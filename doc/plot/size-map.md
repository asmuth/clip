Mapping input data to typographic units
---------------------------------------

Sometimes you may want to map input data to typographic units. Once instance where
this is useful is when creating a scatterplot where the point size depends on
some input datum. Mapping from input data to typographic units is accomplished
using the `<measure-map>` construct. The following mappings are currently
supported:

    ;; map a value in the range [0..1] linearly to min..max
    (linear <min> <max>)

#### Examples

    ;; map 0..1 to 12pt..20pt
    (linear 12pt 20pt)

