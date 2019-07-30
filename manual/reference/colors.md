Colors
======

The `<color>` setting allows you to configure colors. The following ways to
specify colors are supported:

    ;; specify a color by hex code or reference a color from the color palette
    <color-code>

    ;; specify a color by rgb. components should be in the range 0..1
    (rgb <r> <g> <b>)

    ;; specify a color by rgba. components should be in the range 0..1
    (rgba <r> <g> <b> <a>)


#### Examples:

    ;; set the color to green
    color #f00

    ;; set the color to green
    color #ff0000

    ;; set the color to green, 50% alpha
    color #ff000080

    ;; set the color to red
    color (rgb 1.0 0.0 0.0)

    ;; set the color to red, 50% alpha
    color (rgba 1.0 0.0 0.0 0.5)

    ;; usage color number '3' from the color palette
    color 3


Color Palettes
--------------

The color palette allows you to easily reference colors that are frequently used
in the chart. Colors from the color palette are referenced by one-base index
(see above).

You can use one of the built-in color palettes (see below) or define your own
color palette using the `set` directive:

    (set color-palette (<color>...))
    (set color-palette <color-palette-name>)

For example:

    ;; set a custom color palette
    (set color-palette (#06c #c06 #60c))

    ;; use the default 'fnord' color palette
    (set color-palette fnord)


List of default color palettes
-------------------------------
