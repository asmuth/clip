Fonts
=====

The `<font>` setting allows you to configure the font that should be used for
drawing text and labels. In general, the font can be controlled for each label
individually. If no explicit font is specified, the default font will be used.

##### Example: [`charts-custom/custom_font`](/examples/charts-custom/custom_font):
[![custom_font.svg](/examples/charts-custom/custom_font.svg)](/examples/charts-custom/custom_font)


## Custom Fonts

To use a custom font for a label or title, simply specify a font family name or
font filename wherever a `<font>` can be configured. For example, this is how
you can setup a font to be used as the default font.

    ;; use comic sans
    font ("Comic Sans MS")

    ;; use comic sans
    font ("/usr/share/fonts/msttcore/comic.ttf")

Fonts specified by filename are directly loaded from disk. Fonts specified by
font family name are looked up using the system font API. On Linux, fontconfig
is used.

Note that whenever you are specifying a custom font the font is added to the top
of the font stack. If a character is not found in the font, the font selection
logic will still fall back to the next font in the stack. When specifying multiple
font names or files, they will be added to the font stack in order. For more
information on font stacks, see below.



## Default Fonts

The default fonts will be initialized based on your system configuration. For
roman languages the default font is always set to the best available font from
the "Helvetica/Arial" font family.

The default font can be changed using the `set` directive:

    // set the default font to computer modern
    (set font "CMU Serif")


## Font Stacks

The font lookup logic is based on the idea of a "font stack". A font stack consists
of a list of fonts which are tried in order when rendering text. This is required
because full coverage of all scripts and symbols found in unicode is usually only
possible by automatically selecting glyphs from a number of fonts where each
font supports some subset of all the codepoints found in unicode.

For example, a default font stack will usually consist of a font for roman text,
a font for arabic text and another set of fonts for CJK text. The text rendering
code will then automatically select the correct font to use at runtime.

By default, clip will initialize the fonts stack using the default font stacks
supplied by your system. In most cases this useful since specifying all list of fonts
required to render international text by yourself would be cumbersome.

Still, sometimes you might want to disable the default stack. One example of a
situation where this might be required is if you want pixel-to-pixel reproducible
results on different systems.

To disable the default font stack, set the global `font-defaults` property to
`off`:

    ;; disable the default font stack
    (set font-defaults off)

    ;; manually add fonts to the default stack
    (set font "arial.ttf" "KhmerOS.ttf" "STIX-Regular.otf" ..)


Alternatively, add the `--font-defaults off` flag when invoking the `clip` binary

    $ clip \
        --font-defaults off \
        --font-load arial.ttf \
        --font-load KhmerOS.ttf \
        ...

