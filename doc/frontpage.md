<h1>clip (v0.6)</h1>

clip (the _command line illustration processor_) is an open-source command line
program and software library for creating charts and other data-driven
illustrations.

<div style="margin-top: 1.2em;">
  <a href="./documentation/getting-started" style="">Getting Started</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./examples">Examples</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./installation">Download</a>
</div>

<div style="margin-top: 2em; margin-bottom: 3em; background: #f2f7ff; padding: 10pt; max-width: 900px; box-sizing: border-box;">
  <b>BETA:</b> Please note that clip is pre-1.0 software under active development;
  It has reached a "minumum viable product" level of quality, but users should
  expect things to generally be a bit rough around the edges still. To see what
  already works, have a look at the <a href="/examples">Examples</a> page.
</div>

## Documentation

<table>
  <tr>
    <td style="width: 30%;">
      <h3>Introduction</h3>
      <div class="plist" style="margin-bottom: 2em;">
        <a class="prop" href="./getting-started"><span>Getting Started</span></a>
        <a class="prop" href="./installation"><span>Download<span></a>
        <a class="prop" href="./installation"><span>Installation<span></a>
        <a class="prop" href="./usage/command-line"><span>Usage<span></a>
      </div>
    </td>
    <td style="width: 30%;">
      <h3>Colors & Fonts</h3>
      <ul class="index">
        <li><a href="/reference/document-setup">Document Setup</a></li>
        <li><a href="/reference/colors">Colors</a></li>
        <li><a href="/reference/color-maps">Color Maps</a></li>
        <li><a href="/reference/typographic">Typographic Units</a></li>
        <li><a href="/reference/fonts">Fonts</a></li>
        <li><a href="/reference/marker-shapes">Marker Shapes</a></li>
      </ul>
    </td>
    <td style="width: 30%;">
      <h3>Scales & Units</h3>
      <ul class="index">
        <li><a href="/reference/scale-types">Scale Types</a></li>
        <li><a href="/reference/scale-layout">Scale Layout</a></li>
        <li><a href="/reference/format">Number Formatters</a></li>
      </ul>
    </td>
  </tr>
</table>

### List of Commands

<table>
  <tr>
    <td style="width: 30%;">
      <h5>Document Setup</h5>
      <div class="plist" style="margin-bottom: 2em;">
        <a href="#" class="prop"><span><code><strong>set-width</strong></code></span></a>
        <a href="#" class="prop"><span><code><strong>set-height</strong></code></span></a>
        <a href="#" class="prop"><span><code><strong>set-dpi</strong></code></span></a>
      </div>
    </td>
    <td style="width: 30%;">
      <h5>Plotting Data</h5>
      <div class="plist" style="margin-bottom: 2em;">
        <a class="prop" href="/commands/plot/add-axes"><span><code><strong>plot/add-axes</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-areas"><span><code><strong>plot/draw-areas</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-axis"><span><code><strong>plot/draw-axis</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-bars"><span><code><strong>plot/draw-bars</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-errorbars"><span><code><strong>plot/draw-errorbars</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-grid"><span><code><strong>plot/draw-grid</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-labels"><span><code><strong>plot/draw-labels</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-lines"><span><code><strong>plot/draw-lines</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-points"><span><code><strong>plot/draw-points</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-vectors"><span><code><strong>plot/draw-vectors</strong></code></span></a>
        <a class="prop" href="/commands/plot/draw-rectangles"><span><code><strong>plot/draw-rectangles</strong></code></span></a>
      </div>
    </td>
    <td style="width: 30%;">
      <h5>Layout</h5>
      <ul class="index">
        <li><a href="#"><code>layout/push</code></a></li>
        <li><a href="#"><code>layout/pop</code></a></li>
        <li><a href="#"><code>layout/add-margins</code></a></li>
      </ul>
    </td>
  </tr>
  <tr>
    <td style="width: 30%;">
      <h5>Other</h5>
      <div class="plist" style="margin-bottom: 2em;">
        <a href="#" class="prop"><span><code><strong>figure/add-legend</strong></code></span></a>
      </div>
    </td>
    <td style="width: 30%;">
    </td>
    <td style="width: 30%;">
    </td>
  </tr>
</table>


## Community

<ul>
  <li><a href="https://github.com/asmuth/clip">Github</a></li>
  <li><a href="./contributing">Contributing</a></li>
  <li><a href="./license">License</a></li>
</ul>


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


