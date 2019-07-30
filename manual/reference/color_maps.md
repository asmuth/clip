Color Maps
==========

The `<color-map>` setting allows you to map input data to colors. These are the
currently implemented mapping methods:

  - `steps` -- Map values in [0..1] to a set of discrete colors
  - `gradient` -- Map values in [0..1] to a linear color gradient

#### Syntax:

    ;; a 'step' color map
    (steps (<step> <color>)...)

    ;; a 'gradient' color map
    (gradient (<step> <color>)...)


List of built-in color maps
---------------------------

