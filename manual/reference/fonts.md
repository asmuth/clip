Fonts
=====

The `<font>` setting allows you to configure the font that should be used for
drawing text and labels. In general, the font can be controlled for each label
individually. If no explicit font is selected, the corresponding default font
will be used.

##### Example: [`charts-custom/custom_font`](/examples/charts-custom/custom_font):
[![custom_font.svg](/examples/charts-custom/custom_font.svg)](/examples/charts-custom/custom_font)

### Font Lookup

A font is specified by one or more font family names followed by a font style
which can be "Regular", "Medium", "Bold" or "Light". Alternatively you can use
one of the default font stacks.

    (<font-name>... <font-style>)

For example:

    ;; set the default font to "Arial"
    (set font "Arial" "Regular")

    ;; set the default em font to Helvetica Neue or Arial Bold
    (set font-emphasis "Helvetica Neue" "Arial" "Bold")

    ;; set the default fonts to the standard roman serif fonts
    (set font roman-serif)
    (set font roman-serif-bold)


### Global Font Configuration

There are three global default fonts that can be changed.

  - `font` - This is the default font used for drawing labels
  - `font-emphasis` - This is the default font used for drawing emphasized labels, such as titles
  - `font-symbols` - This is the default font used for drawing unicode 'graphical' symbols

To change the global default fonts, use the `set` command:

    (set font <font-names>)
    (set font-emphasis <font-names>)
    (set font-symbols <font-names>)


### Built-In Font Stacks

Instead of referencing specific fonts by name, you can use one of these built-in
font stacks:

  - `roman-sans` (Arial, Helvetica, Helvetica Neue)
  - `roman-sans-medium` (Arial Medium, Helvetica Medium, Helvetica Neue Medium)
  - `roman-sans-bold` (Arial Bold, Helvetica Bold, Helvetica Neue Bold)
  - `roman-serif` (CMU Serif)
  - `roman-serif-medium` (CMU Serif Medium)
  - `roman-serif-bold` (CMU Serif Bold)
