clip
====

clip (the _command line illustration processor_) is an open-source command line
program and software library for creating charts and other data-driven
illustrations.

<h4>
  <a href="https://clip-lang.org/getting-started">Getting Started</a> &middot;
  <a href="https://clip-lang.org/examples">Examples</a> &middot;
  <a href="https://clip-lang.org">Documentation</a>
</h4>


Introduction
------------

In essence, clip is an automated drawing program; it reads a text file containing
a description of the chart or diagram and produces an image from it. This is best
explained by example, so here is how to draw a simple line chart using clip:

```css
class: plot;

limit-x: 0 7200;
limit-y: 0 100;

axes {
  font: "Roboto";
  position: left bottom;
  label-placement-x: linear-interval(900 900 7000);
  label-format-x: datetime("%H:%M:%S");
}

lines {
  data: "test/testdata/timeseries.csv";
  stroke-width: 0.8pt;
  color: #06c;
}
```

The input file from above (`example.clp`) can be processed with clip using the
following command:

    $ clip --export output.svg example.clp

This is the resulting SVG file (`output.svg`):

<div align="center">
  <a href="https://clip-lang.org/examples/charts-scientific/multiple_y_axes">
    <img src="/doc/demo.svg" width="80%" alt="Demo Chart" />
  </a>
</div>

Note that this example is only intended to give you an idea of what the syntax
looks like; for an in-depth description of all parameters, please refer to
[the documentation](https://clip-lang.org). More examples can be found on
[the examples page](https://clip-lang.org/examples).


Documentation
-------------

You can find the full documentation and more examples at [clip-lang.org](https://clip-lang.org/).


Installation
------------

#### Compile from source

To build clip, you need an up-to-date C++ compiler, cmake, fmtlib, libharfbuzz,
libfreetype and cairo. Then run:

    $ cmake .
    $ make

To install the `clip` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test


For detailed installation instructions, have a look at the [Installation](https://clip-lang.org/documentation/installation/)
page.


FAQ
---

#### I'm getting build errors when compiling the code

clip is written in C++17 and requires a reasonably modern C++ compiler and standard
library. In most cases where the code doesn't build it's due to some problem with
the local build environment. Still, we always appreciate bug reports via Github
Issues so that we can improve our build system.

#### The test suite fails on my machine because text placement is slightly offset

The test suite requires that you have the original Microsoft Arial TTF files
installed on your machine. To verify that this is the case, run `fc-match
'Arial,Helvetica,Helvetica Neue:style=Regular,Roman'` and check that it returns
the correct 'arial.ttf' file.

#### What happened to the project name?

The project was started in 2011 and was initially called "FnordMetric". The
first version from 8 years ago also included facilities for storing and
transforming data in addition to the charting code. Over time, the data
processing parts were removed, leaving only the plotting code. However, as a
consequence, most of the search queries for the project name would return
outdated information, resulting in a generally confusing and stale-feeling
situation. The best solution seemed to be to rename the project and so it was
renamed to "clip".


Acknowledgements
----------------

  - Text rendering is based on the libre [freetype](https://www.freetype.org/)
    and [harfbuzz](https://harfbuzz.org) stack.

  - The structure of the plot elements is based on ideas from [The Grammar of
    Graphics](https://www.springer.com/gp/book/9780387245447) and the
    [ggplot2](https://ggplot2.tidyverse.org/) library.

  - A number of concepts in clip are modelled after the
    [CSS specification](https://www.w3.org/TR/CSS2/)


License
-------

    clip -- The command line illustration processor
    https://clip-lang.org

    Copyright (c) 2020, Paul Asmuth, Laura Schlimmer
    All rights reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
