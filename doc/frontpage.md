<h1>Documentation</h1>

clip (the _command line illustration processor_) is an open-source command line
program and software library for creating charts and other data-driven
illustrations.

In essence, clip consists of a library of drawing commands. This command library
includes high-level building blocks for creating common chart types as well as
lower-level drawing primitives such as markers, arrows and lines.

When called from the command line, clip reads input data in text and CSV format
and produces the output chart as a SVG or PNG file. Additionaly, clip is available
as the libclip software library which can be embedded into any application that
supports a C FFI.

<p>
  <a href="./getting-started" style="">Getting Started</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./examples">Examples</a>
  <span style="margin: 0 .3em">·</span>
  <a href="./installation">Download</a>
</p>

<div style="margin-top: 1em; margin-bottom: 2em; background: #f2f7ff; padding: 10pt; max-width: 50em; box-sizing: border-box;">
  <b>BETA:</b> Please note that clip is pre-1.0 software under active development;
  It has reached a "minumum viable product" level of quality, but users should
  expect things to generally be a bit rough around the edges and subject to change.
  To see what already works, have a look at the <a href="/examples">Examples</a> page.
</div>


References
----------

<ul>
  <li><a href="./getting-started">Getting Started</a></li>
  <li><a href="./commands">API Reference</a></li>
  <li><a href="./examples">Examples</a></li>
  <li><a href="./installation">Download</a></li>
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


