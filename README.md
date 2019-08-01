fviz
====

fviz is a text-based data visualization program. It generates custom charts
and other data-driven illustrations from simple expressions.

<h4>
  <a href="https://fviz.org/examples">Examples</a> &middot;
  <a href="https://fviz.org">Documentation</a>
</h4>


Features
--------

**BETA:** Please note that fviz is pre-1.0 software under active development;
It is usable, but things might generally still be a bit rough around the edges.
To see what already works, have a look at the [Examples](...) page.

- Supports most common chart types through a library of composable elements
- All elements are highly customizable
- High quality SVG output; suitable for web and print
- Supports custom fonts and complex text shaping
- Very simple command line interface and input syntax
- Available as a standalone library that can be embedded into any application
- Output files can be edited in vector graphic editors like inkscape
- Supports the CSV, TSV and JSON input data formats
- Licensed unter the Apache 2.0 license


Example
-------

Being a highly visual tool, fviz is best explained by example. So here is a fviz
file that defines a simple scatterplot (`example_chart.fvz`). Note that this
example is only intended to give you an idea of what the syntax looks like and to
get you started quickly; for an in-depth description of all parameters, please refer
to the documentation.

    (chart/scatterplot
        data-x (csv tests/testdata/gauss2d.csv x)
        data-y (csv tests/testdata/gauss2d.csv y)
        limit-x (0 400)
        limit-y (0 200)
        axes (bottom left)
        grid (color #fff)
        background #eee
        border none)

Here is how you can run the above example file through fviz:

    $ fviz --in example_chart.fvz --out example_chart.svg

When running the example locally, you can use your own input CSV file, or you
can download the example CSV file [from here](/tests/testdata/gauss2d.csv).
If everything works, you should get an output file similar to the one below
(`example_chart.svg`):

[![A simple scatterplot](/examples/charts-basic/scatterplot.svg)](https://fviz.org/examples/charts-basic/scatterplot)

More examples can be found on [the examples page](https://fviz.org/examples).
For a more detailed introduction to fviz, see the [Getting Started](https://fviz.org/documentation/getting-started) page.
If you have any questions please don't hesitate to reach out via [GitHub issues](https://github.com/asmuth/fviz).


Building
--------

To build fviz, you need an up-to-date C++ compiler, cmake, fmtlib, libharfbuzz,
libfreetype and cairo. Run:

    $ cmake .
    $ make -j

To install the `fviz` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test


For detailed installation instructions, have a look at the [Installation](https://fviz.org/documentation/installation/)
page.


Acknowledgements
----------------

Parts of fviz were inspired by ideas from the "Grammar of Graphics" [0] and the
ggplot2 project.

[0] Wilkinson, L. (1999). The Grammar of Graphics (Springer)


Example Gallery
---------------

Below are more examples to show you what's possible. Each example links to the
source code that generated it. Even more examples can be found on the
[Examples](https://fviz.org/examples) page.

---
#### Example: [`examples/charts-basic/linechart_timeseries`](https://fviz.org/examples/charts-basic/linechart_timeseries)
[![linechart_timeseries.svg](/examples/charts-basic/linechart_timeseries.svg)](https://fviz.org/examples/charts-basic/linechart_timeseries)

---
#### Example: [`examples/charts-basic/barchart_groups`](https://fviz.org/examples/charts-basic/barchart_groups)
[![barchart_groups.svg](/examples/charts-basic/barchart_groups.svg)](https://fviz.org/examples/charts-basic/barchart_groups)

---
#### Example: [`examples/charts-basic/scatterplot_with_labels`](https://fviz.org/examples/charts-basic/scatterplot_with_labels)
[![scatterplot_with_labels.svg](/examples/charts-basic/scatterplot_with_labels.svg)](https://fviz.org/examples/charts-basic/scatterplot_with_labels)

---
#### Example: [`examples/charts-basic/custom_font`](https://fviz.org/examples/charts-basic/custom_font)
[![custom_font.svg](/examples/charts-basic/custom_font.svg)](https://fviz.org/examples/charts-basic/custom_font)

---
#### Example: [`examples/charts-scientific/vectorfield`](https://fviz.org/examples/charts-scientific/vectorfield)
[![vectorfield.svg](/examples/charts-scientific/vectorfield.svg)](https://fviz.org/examples/charts-scientific/vectorfield)

---
#### Example: [`examples/charts-scientific/line_markers`](https://fviz.org/examples/charts-scientific/line_markers)
[![line_markers.svg](/examples/charts-scientific/line_markers.svg)](https://fviz.org/examples/charts-scientific/line_markers)

---
#### Example: [`examples/charts-scientific/errorbars_log`](https://fviz.org/examples/charts-scientific/errorbars_log)
[![errorbars_log.svg](/examples/charts-scientific/errorbars_log.svg)](https://fviz.org/examples/charts-scientific/errorbars_log)

---
#### Example: [`examples/charts-scientific/multiple_y_axes`](https://fviz.org/examples/charts-scientific/multiple_y_axes)
[![multiple_y_axes.svg](/examples/charts-scientific/multiple_y_axes.svg)](https://fviz.org/examples/charts-scientific/multiple_y_axes)

---
#### Example: [`examples/charts-scientific/scatterplot_colors`](https://fviz.org/examples/charts-scientific/scatterplot_colors)
[![scatterplot_colors.svg](/examples/charts-scientific/scatterplot_colors.svg)](https://fviz.org/examples/charts-scientific/scatterplot_colors)

---
#### Example: [`examples/charts-scientific/barchart_ranges`](https://fviz.org/examples/charts-scientific/barchart_ranges)
[![barchart_ranges.svg](/examples/charts-scientific/barchart_ranges.svg)](https://fviz.org/examples/charts-scientific/barchart_ranges)

---
#### Example: [`examples/charts-editorial/linechart_with_labels`](https://fviz.org/examples/charts-editorial/linechart_with_labels)
[![linechart_with_labels.svg](/examples/charts-editorial/linechart_with_labels.svg)](https://fviz.org/examples/charts-editorial/linechart_with_labels)

---
#### Example: [`examples/charts-editorial/chart_i18n`](https://fviz.org/examples/charts-editorial/chart_i18n)
[![chart_i18n.svg](/examples/charts-editorial/chart_i18n.svg)](https://fviz.org/examples/charts-editorial/chart_i18n)

---
#### Example: [`examples/charts-editorial/stacked_areas`](https://fviz.org/examples/charts-editorial/stacked_areas)
[![stacked_areas.svg](/examples/charts-editorial/stacked_areas.svg)](https://fviz.org/examples/charts-editorial/stacked_areas)

---
#### Example: [`examples/charts-editorial/barchart_horizontal`](https://fviz.org/examples/charts-editorial/barchart_horizontal)
[![barchart_horizontal.svg](/examples/charts-editorial/barchart_horizontal.svg)](https://fviz.org/examples/charts-editorial/barchart_horizontal)


License
-------

    fviz -- Open-Source Data Visualization Toolkit 
    https://fviz.org

    Copyright (c) 2018, Paul Asmuth, Laura Schlimmer
    All rights reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
