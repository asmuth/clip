Document Setup
==============

The `set` directive allows you to configure a number of global default options.
The general syntax for setting global options is:

    (set <option> <value>)

Below is a list of all currently supported global options, please see the remainder
of this page for detailed documentation on what these do.

**Global Options:**
[`width`](#option-width)
[`height`](#option-height)
[`dpi`](#option-dpi)
[`font`](#option-font)
[`font-defaults`](#option-font-defaults)
[`font-size`](#option-font-size)
[`background-color`](#option-background-color)
[`foreground-color`](#option-foreground-color)
[`margin-top`](#option-margin-top)
[`margin-right`](#option-margin-right)
[`margin-bottom`](#option-margin-bottom)
[`margin-left`](#option-margin-left)
[`color-palette`](#option-color-palette)
[`text-language`](#option-text-language)
[`text-script`](#option-text-script)



Width & Height
--------------
<a id="option-width"></a>
<a id="option-height"></a>

To configure the output dimensions, use the global `width` and `height` options:

    (set width <measure>)
    (set height <measure>)

For example:

    (set width 1920px)
    (set height 1200px)


Scaling (DPI)
-------------
<a id="option-dpi"></a>

The `dpi` setting controls the mapping of typographic units to screen (pixel)
coordinates. The default dpi value is 96 for consistency with CSS.

    (set dpi <value>)

For example:

    (set dpi 240)


Font & Font Size
----------------
<a id="option-font"></a>
<a id="option-font-defaults"></a>
<a id="option-font-size"></a>

The `font` and `font-size` settings control the default font and font size. The
`font-defaults` setting can be used to disable default font loading. For more
information on these options please see the [Fonts](/documentation/fonts) page.

    (set font <font-name>)
    (set font-defaults <on|off>)
    (set font-size <measure>)

For example:

    (set font "Comic Sans MS")
    (set font-size 16pt)


Foreground & Background Colors
------------------------------
<a id="option-foreground-color"></a>
<a id="option-background-color"></a>

The `foreground-color` and `background-color` settings control the document
background color and the default color used to draw lines and labels.

    (set background-color <color>)
    (set foreground-color <color>)

For example:

    ;; welcome to the matrix
    (set background-color #000)
    (set foreground-color #0f0)


Margins
-------
<a id="option-color-margin-top"></a>
<a id="option-color-margin-right"></a>
<a id="option-color-margin-bottom"></a>
<a id="option-color-margin-left"></a>

The margin options can be used to add a global margin to the document.

    (set margin-top <measure>)
    (set margin-right <measure>)
    (set margin-bottom <measure>)
    (set margin-left <measure>)

For example:

    ;; 2em margin
    (set margin-top 2em)
    (set margin-right 2em)
    (set margin-bottom 2em)
    (set margin-left 2em)


Color Palette
-------------
<a id="option-color-palette"></a>

The `color-palette` option is used to set the default color palette. For more
information, please see the [Colors](/documentation/colors) page.

    (set color-palette (<color>...))
    (set color-palette <color-palette-name>)

For example:

    ;; set a custom color palette
    (set color-palette (#06c #c06 #60c))

    ;; use the default 'fnord' color palette
    (set color-palette fnord)



Text Script & Language
----------------------
<a id="option-color-text-script"></a>
<a id="option-color-text-language"></a>

The `text-script` and `text-language` are used to set the default text language
and script to be used in text rendering. Setting these options should usually
not required.

    (set text-language <value>)
    (set text-script <value>)

For example:

    ;; set the default script to arabic
    (set text-script "Arabic")

