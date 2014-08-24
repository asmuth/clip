FnordMetric
===========

FnordMetric extends standard SQL; it allows to express queries that return SVG
charts rather than tables. Turning a query result into a chart is literally one
line of code.

Documentation: [fnordmetric.io](http://fnordmetric.io/documentation/)

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/fnordmetric.png) ](http://travis-ci.org/paulasmuth/fnordmetric)

[ ![Screenshot](https://github.com/paulasmuth/fnordmetric/raw/v1.0-alpha/doc/preview3.png) ](http://github.com/paulasmuth/fnordmetric)

If you're like me you are probably thinking something along the lines of "Does
the world really need yet another data visualization tool?" right now. Surely,
there already is gnuplot and about a bazillion javascript libraries that let you
draw pretty charts.

Most of the existing open source tools require you to provide the input data in
some specific, custom format. I reckon this is not ideal. It means you have to
write a heap of reptitive glue code (or sed incantations if that's your thing)
to mangle your input data into that format. If you run a lot of ad-hoc queries
you have to waste a significant amount of time on this boring legwork that could
much better be spent on interesting tasks.

FnordMetric aims to fix that by extending standard SQL; it allows you to express
queries that return charts rather than tables.

An example says more than a thousand words so here is how you generate a shiny
timeseries chart from a csv file using nothing else than plain old SQL.
   ...

Documentation
-------------

You can find the full FnordMetric Documentation at http://fnordmetric.io/


Contributions
-------------

Ways to contribute to FnordMetric:

  + report bugs or suggest new features
  + suggest or implement new options for existing charts or new chart types
  + improve the documentation
  + publish howtos, tutorials and videos
  + add a new default stylesheet (requires css knowledge)
  + add a new language binding (requires minimal c++ knowledge)
  + add a new query backend (requires c++ knowledge)
  + add a new render target (requires c++ knowledge)
  + add some new features to the query engine (requires c++ knowledge)
  + many more! improve all the things :)

#### Other Open Source Software included in FnordMetric

  + Christian Parpart's xzero -- HTTP library (http://github.com/xzero)
  + Marijn Haverbeke's CodeMirror --- javascript code editor (https://github.com/marijnh/codemirror)

#### Individial Contributors to FnordMetric

**Version 2.0.0**
  + Christian Parpart (http://github.com/trapni)

**Version 0.0.8 - v1.2.9**
 + Simon Menke (http://github.com/fd),
 + Bruno Michel (http://github.com/nono),
 + Marco Borromeo (http://github.com/mborromeo),
 + Leo Lou (http://github.com/l4u)
 + Andy Lindeman (http://github.com/alindeman),
 + Jurriaan Pruis (http://github.com/jurriaan),
 + Kacper Bielecki (http://github.com/kazjote),
 + John Murray (http://github.com/JohnMurray),
 + Lars Gierth (http://github.com/lgierth),
 + Ross Kaffenberger (http://github.com/rossta),
 + Kunal Modi (http://github.com/kunalmodi),
 + Michael Fairchild (http://github.com/fairchild),
 + James Cox (http://github.com/imajes),
 + Pieter Noordhuis (http://github.com/pietern),
 + Tadas Ščerbinskas (http://github.com/tadassce),
 + Sebastian Korfmann (http://github.com/skorfmann)

To contribute, please fork this repository, make your changes and run the 
specs, commit them to your github repository and send me a pull request.
Need help, head on over to our [Google Groups][1]  page to discuss any ideas
that you might have.

License
-------

Copyright (c) 2011-2014 Paul Asmuth

FnordMetric is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FnordMetric is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FnordMetric. If not, see <http://www.gnu.org/licenses/>.


  [1]: http://groups.google.com/group/fnordmetric
  [2]: http://www.screenr.com/KiJs
  [3]: https://secure.travis-ci.org/paulasmuth/fnordmetric.png
  [4]: http://travis-ci.org/paulasmuth/fnordmetric
