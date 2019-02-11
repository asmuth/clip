PlotFX
======

PlotFX is a command line tool and library for plotting data. The appearance of
plots can be customized using a lightweight syntax that is very similar to CSS.

You can run PlotFX from the command line, where it generates SVG, PNG or PDF
output files. Additionally, you can embed it into your own application using the
C API.

<h4>
  <a href="https://plotfx.org/documentation/getting-started">Getting Started</a> &middot;
  <a href="https://plotfx.org/examples">Examples</a> &middot;
  <a href="https://plotfx.org">Documentation</a>
</h4>

<p align="center">
  <br>
  <img src="/examples/other/demo.svg" width="90%">
</p>

<p align="center">
  <i>A collection of examples generated with PlotFX.</i>
  <br><br>
</p>


Project Status
--------------

**WARNING**: The master branch recently underwent a significant refactoring and the
stability is currently beta/experimental. Maybe not even that. It will take a few
more weeks to stabilize and document everything.


Getting Started
---------------

Here is how to draw a simple timeseries chart using plotfx:

    $ plotfx --in example_chart.ptx --out example_chart.svg

Output File (`example_chart.svg`):

[![A simple line chart](/examples/linecharts/simple_timeseries.svg)](./examples/linecharts/simple_timeseries.ptx)

Input File (`example_chart.ptx`):

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


For more examples, please see [the examples page](https://plotfx.org/examples).


Building
--------

To build PlotFX, you need an up-to-date C++ compiler, cmake, libharfbuzz and
libfreetype. Run:

    $ cmake .
    $ make

To install the `plotfx` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test


Acknowledgements
----------------

PlotFX is the successor of the FnordMetric ChartSQL project by the same authors.

Parts of the new chart specification syntax and semantics were inspired by ideas
from the "Grammar of Graphics" [0] and the ggplot2 project.

[0] Wilkinson, L. (1999). The Grammar of Graphics (Springer)

License
-------

    The PlotFX License (https://plotfx.org)
    Copyright (c) 2018, Paul Asmuth, Laura Schlimmer
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
