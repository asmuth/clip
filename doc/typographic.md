Typographic Units
=================

A number of options take a typographic unit as an argument. These options are
marked with the type `<measure>`.

The following typographic units are currently accepted. Users are encouraged to
specify typographic measurements using the "em" or "pt" units. Doing so will
allow you to scale the output resolution of the chart using the global `dpi`
option.

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
