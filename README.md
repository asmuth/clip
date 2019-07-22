fviz
====

fviz is a text-based data visualization program. It generates custom charts,
dashboards and other illustrations from simple expressions.

You can run fviz from the command line, where it generates SVG, PNG and PDF
output files. Additionally, you can embed it into your own application using the
C API.

<h4>
  <a href="https://fviz.org/examples">Examples</a> &middot;
  <a href="https://fviz.org">Documentation</a>
</h4>


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


Acknowledgements
----------------

fviz is the successor of the FnordMetric ChartSQL project by the same authors.

Parts of the new chart specification semantics were inspired by ideas from the
"Grammar of Graphics" [0] and the ggplot2 project.

[0] Wilkinson, L. (1999). The Grammar of Graphics (Springer)


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
