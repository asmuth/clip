# clip

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


<h4>
  <a href="https://docs.clip-lang.org/getting-started">Getting Started</a> &middot;
  <a href="https://docs.clip-lang.org/examples">Examples</a> &middot;
  <a href="https://docs.clip-lang.org">Documentation</a>
</h4>


Example
-------

Being a highly visual tool, clip is best explained by example. So here is how to
draw a simple line chart using clip:

    $ clip --in example_chart.clp --out example_chart.svg

Output File (`example_chart.svg`):
[![A simple scatterplot](/examples/charts-basic/demo_timeseries.svg)](https://docs.clip-lang.org/examples/charts-basic/demo_timeseries)

Input File (`example_chart.clp`):

    (plot
      axes (bottom left)
      axis-y-label-format (scientific)
      axis-x-label-format (datetime "%H:%M:%S")
      axis-x-label-placement (linear-align 1800)
      lines (
        data-x (csv "tests/testdata/measurement.csv" time)
        data-y (csv "tests/testdata/measurement.csv" value1)
        color #06c))

Note that this example is only intended to give you an idea of what the syntax
looks like and to get you started quickly; for an in-depth description of all
parameters, please refer to [the documentation](https://docs.clip-lang.org).

More examples can be found on [the examples page](https://docs.clip-lang.org/examples).

If you have any questions please don't hesitate to reach out via [GitHub issues](https://github.com/asmuth/clip).


Documentation
-------------

You can find the full documentation at https://docs.clip-lang.org/


Example Gallery
---------------

A list of examples can be found on the [Examples](https://docs.clip-lang.org/examples)
page.


Building
--------

To build clip, you need an up-to-date C++ compiler, cmake, fmtlib, libharfbuzz,
libfreetype and cairo. Run:

    $ cmake .
    $ make -j

To install the `clip` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test


For detailed installation instructions, have a look at the [Installation](https://docs.clip-lang.org/documentation/installation/)
page.


Acknowledgements
----------------

  - The structure of the plot elements is based on ideas from [The Grammar of
    Graphics](https://www.springer.com/gp/book/9780387245447) and the
    [ggplot2](https://ggplot2.tidyverse.org/) library.

  - A number of concepts in clip are heavily guided by the
    [CSS specification](https://www.w3.org/TR/CSS2/)

  - Text rendering is based on the libre [freetype](https://www.freetype.org/)
    and [harfbuzz](https://harfbuzz.org) stack.

  - Some naming choices are definitely inspired by Eddie Kohler's
    [click](https://github.com/kohler/click) software defined networking library


License
-------

    clip -- The command line illustration processor
    https://clip-lang.org

    Copyright (c) 2018, Paul Asmuth, Laura Schlimmer
    All rights reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
