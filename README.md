fviz
====

fviz is a text-based drawing program. It generates custom charts, figures and other
visualizations from simple expressions.

You can run fviz from the command line, where it generates SVG, PNG and PDF
output files. Additionally, you can embed it into your own application using the
C API.

<h4>
  <a href="https://fviz.org/documentation/getting-started">Getting Started</a> &middot;
  <a href="https://fviz.org/examples">Examples</a> &middot;
  <a href="https://fviz.org">Documentation</a>
</h4>

---

<p align="center">
  <br>
  <img src="/examples/other/demo2.svg">
</p>

<p align="center">
  <i>A collection of examples generated with fviz.</i>
  <br><br>
</p>


Project Status
--------------

**WARNING**: The current project status is "beta", i.e. the software works, but
it's still rough around the edges. It is also severely underdocumented. I'm working
on polishing everything up and filling in the blanks in the documentation, but we're
not quite there yet...


Example / Demo
--------------

Being a highly visual tool, fviz is best explained by example. So here is a fviz
file that defines a simple timeseries chart (`example_chart.fvz`). Note that this
example is only intended to give you an idea of what the syntax looks like and to
get you started quickly; for an in-depth description of all parameters, please refer
to the documentation.

    width: 1200px;
    height: 280px;

    scale-y-min: 0;
    scale-y-max: 140;

    lines {
      xs: csv('measurement.csv', time);
      ys: csv('measurement.csv', value2);
      colors: #06c;
    }

    axis {
      position: bottom;
      format: datetime("%H:%M:%S");
    }

    axis {
      position: left;
      layout: linear(20);
    }

Here is how you can run the above example file through fviz:

    $ fviz --in example_chart.fvz --out example_chart.svg

When running the example locally, you can use your own input CSV file, or you
can download the example CSV file [from here](/tests/testdata/measurement.csv).
If everything works, you should get an output file similar to  the one below
(`example_chart.svg`):

[![A simple line chart](/examples/linecharts/simple_timeseries.svg)](./examples/linecharts/simple_timeseries.fvz)

More examples can be found on [the examples page](https://fviz.org/examples).
For a more detailed introduction to fviz, see the [Getting Started](https://fviz.org/documentation/getting-started) page.
If you have any questions please don't hesitate to reach out via [GitHub issues](https://github.com/fviz/fviz).


Building
--------

To build fviz, you need an up-to-date C++ compiler, cmake, libharfbuzz and
libfreetype. Run:

    $ cmake .
    $ make -j

To install the `fviz` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test


Acknowledgements
----------------

fviz is the successor of the FnordMetric ChartSQL project by the same authors.

Parts of the new chart specification syntax and semantics were inspired by ideas
from the "Grammar of Graphics" [0] and the ggplot2 project.

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
