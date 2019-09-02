<h1>Documentation</h1>

clip (the _command line illustration processor_) is an open-source command line
program and software library for creating charts and other data-driven
illustrations.

In essence, clip consists of a library of composable graphical 'elements'. This
element library includes high-level building blocks for creating common chart types
as well as lower-level drawing primitives such as markers, arrows and lines. Users
create custom illustrations by combining and styling these elements.

When called from the command line, clip reads input data in text and CSV format
and produces the output chart as a SVG or PNG file. Additionaly, clip is available
as the libclip software library which can be embedded into any application that
supports a C FFI.

<div style="margin-top: 1.2em;">
  <a href="./documentation/getting-started" style="">Getting Started</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./examples">Examples</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./installation">Download</a>
</div>

<div style="margin-top: 2em; background: #f2f7ff; padding: 10pt; max-width: 900px; box-sizing: border-box;">
  <b>BETA:</b> Please note that clip is pre-1.0 software under active development;
  It has reached a "minumum viable product" level of quality, but users should
  expect things to generally be a bit rough around the edges still. To see what
  already works, have a look at the <a href="/examples">Examples</a> page.
</div>


## Quick Start

- [Getting Started](./getting-started)
- [Download](./installation)
- [Installation](./installation)


## Usage

- [Command Line](./usage/command-line)


## Reference

- [Document Setup](/reference/document-setup)
- [Colors](/reference/colors)
- [Color Maps](/reference/color-maps)
- [Typographic Units](/reference/typographic)
- [Scale Types](/reference/scale-types)
- [Scale Layout](/reference/scale-layout)
- [Formatters](/reference/format)
- [Marker Shapes](/reference/marker-shapes)
- [Fonts](/reference/fonts)


## List of Elements

Element | Description
------- | -----------
[`plot`](elements/plot) | The plot element is the basic container object used to create charts.
[`plot/areas`](elements/plot/areas) | The plot/areas element is used to create area charts.
[`plot/axis`](elements/plot/axis) | The plot/axis element is used to draw a labelled 'axis'.
[`plot/bars`](elements/plot/bars) | The plot/bars element is used to create bar charts.
[`plot/errorbars`](elements/plot/errorbars) | The plot/errorbars element is used to draw error bars.
[`plot/grid`](elements/plot/grid) | The plot/grid element is used to draw a background grid.
[`plot/labels`](elements/plot/labels) | The plot/labels element is used to draw labels at arbitrary positions.
[`plot/lines`](elements/plot/lines) | The plot/lines element is used to create line charts.
[`plot/points`](elements/plot/points) | The plot/points element is used to create scatterplots.
[`plot/vectors`](elements/plot/vectors) | The plot/vectors element is used to create vector "field" plots.
[`plot/rectangles`](elements/plot/rectangles) | The plot/rectangles element is used to create tiled plots.
[`legend`](elements/legend) | The legend element is a container object for legends.
[`legend/item`](elements/legend/item) | The legend/item element is used to draw a legend item.
[`layout/box`](elements/layout/box) | The layout/box element is the basic box layout element.


## Acknowledgements

  - The structure of the plot elements is based on ideas from [The Grammar of
    Graphics](https://www.springer.com/gp/book/9780387245447) and the
    [ggplot2](https://ggplot2.tidyverse.org/) library.

  - A number of concepts in clip are heavily guided by the
    [CSS specification](https://www.w3.org/TR/CSS2/)

  - Text rendering is based on the libre [freetype](https://www.freetype.org/)
    and [harfbuzz](https://harfbuzz.org) stack.

  - Some naming choices are definitely inspired by Eddie Kohler's
    [click](https://github.com/kohler/click) software defined networking library


## Community

- [Github](https://github.com/asmuth/clip)
- [Contributing](./contributing)
- [License](./license)
