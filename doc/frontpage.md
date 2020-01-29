<h1>clip (v0.6)</h1>

clip (the _command line illustration processor_) is an open-source command line
program and software library for creating charts and other data-driven
illustrations.

<div style="margin-top: 1.2em;">
  <a href="./getting-started" style="">Getting Started</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./examples">Examples</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./installation">Download</a>
</div>

<div style="margin-top: 2em; margin-bottom: 3em; background: #f2f7ff; padding: 10pt; max-width: 900px; box-sizing: border-box;">
  <b>BETA:</b> Please note that clip is pre-1.0 software under active development;
  It has reached a "minumum viable product" level of quality, but users should
  expect things to generally be a bit rough around the edges and subject to change.
  To see what already works, have a look at the <a href="/examples">Examples</a> page.
</div>

## Documentation

<h3>Introduction</h3>
<ul class="index">
  <li><a href="./getting-started">Getting Started</a></li>
  <li><a href="./installation">Download</a></li>
  <li><a href="./installation">Installation</a></li>
  <li><a href="./usage/command-line">Usage</a></li>
</ul>

<h3>General Concepts</h3>
<ul class="index">
  <li><a href="/reference/document-setup">Document Setup</a></li>
  <li><a href="/reference/colors">Colors</a></li>
  <li><a href="/reference/color-maps">Color Maps</a></li>
  <li><a href="/reference/typographic">Typographic Units</a></li>
  <li><a href="/reference/fonts">Fonts</a></li>
  <li><a href="/reference/marker-shapes">Marker Shapes</a></li>
  <li><a href="/reference/scale-types">Scale Types</a></li>
  <li><a href="/reference/scale-layout">Scale Layout</a></li>
  <li><a href="/reference/format">Number Formatters</a></li>
</ul>


List of Commands
----------------

<h5>Document Setup</h5>
<ul class="index">
  <li><a href="#" class="prop"><code><strong>width</strong></code></a></li>
  <li><a href="#" class="prop"><code><strong>height</strong></code></a></li>
  <li><a href="#" class="prop"><code><strong>dpi</strong></code></a></li>
</ul>

<h5>Plotting Data</h5>
<ul class="index">
  <li><a href="/commands/plot/axes"><code><strong>plot/axes</strong></code></a></li>
  <li><a href="/commands/plot/areas"><code><strong>plot/areas</strong></code></a></li>
  <li><a href="/commands/plot/axis"><code><strong>plot/axis</strong></code></a></li>
  <li><a href="/commands/plot/bars"><code><strong>plot/bars</strong></code></a></li>
  <li><a href="/commands/plot/errorbars"><code><strong>plot/errorbars</strong></code></a></li>
  <li><a href="/commands/plot/grid"><code><strong>plot/grid</strong></code></a></li>
  <li><a href="/commands/plot/labels"><code><strong>plot/labels</strong></code></a></li>
  <li><a href="/commands/plot/lines"><code><strong>plot/lines</strong></code></a></li>
  <li><a href="/commands/plot/points"><code><strong>plot/points</strong></code></a></li>
  <li><a href="/commands/plot/vectors"><code><strong>plot/vectors</strong></code></a></li>
  <li><a href="/commands/plot/rectangles"><code><strong>plot/rectangles</strong></code></a></li>
</ul>

<h5>Other</h5>
<ul class="index">
  <li><a href="/commands/figure/legend"><code><strong>figure/legend</strong></code></a></li>
</ul>


Community
---------

<ul>
  <li><a href="https://github.com/asmuth/clip">Github</a></li>
  <li><a href="./contributing">Contributing</a></li>
  <li><a href="./license">License</a></li>
</ul>


Acknowledgements
----------------

  - Text rendering is based on the libre [freetype](https://www.freetype.org/)
    and [harfbuzz](https://harfbuzz.org) stack.

  - The structure of some of the plot elements is based on ideas from [The Grammar of
    Graphics](https://www.springer.com/gp/book/9780387245447) and the
    [ggplot2](https://ggplot2.tidyverse.org/) library.

  - A number of concepts in clip are modelled after the
    [CSS specification](https://www.w3.org/TR/CSS2/)


