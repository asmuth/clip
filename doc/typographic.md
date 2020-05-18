Typographic Units
=================

A number of options take a typographic unit as an argument. These options are
marked with the type `<measure>`.

The following typographic units are currently accepted:

  - `px` – Pixels
  - `mm` – Millimeters
  - `pt` – Typographic points
  - `em` – Typographic "em" size
  - `%`  – Relative to the enclosing element

#### Examples

    ;; 10 pixels
    10px

    ;; 40mm
    40mm

    ;; 16 points
    16pt

    ;; 2 "em"
    2em

    ;; 50% of the parent
    50%


"Responsive" Charts
-------------------

Users are encouraged to specify typographic measurements using the "em" or "pt"
units. Doing so will allow you to scale the output resolution of the chart using
the global `dpi` option.


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

