FnordMetric
===========

FnordMetric let's you collect and visualize timeseries data using only SQL. It
enables you to build beautiful real-time analytics dashboards within minutes.

Documentation: [fnordmetric.io](http://fnordmetric.io/documentation/)

[ ![Screenshot](https://github.com/paulasmuth/fnordmetric/raw/v1.0-alpha/doc/preview3.png) ](http://github.com/paulasmuth/fnordmetric)


#### FnordMetric ChartSQL

FnordMetric ChartSQL allows you to write SQL queries that return charts instead
of tables. The charts are rendered as SVG vector graphics and can easily be
embedded into any website and customized with css in order to build beautiful
dashboards.

You can execute ChartSQL queries from the command line against a number of
external sources like CSV files or a MySQL database.


#### FnordMetric Server

Fnordmetric Server is a standalone HTTP server application. It exposes a web UI
and a HTTP API to run ChartSQL queries and collect timeseries data. You can use
fnordmetric-server as a one-stop solution for metric collection and charting.
Since fnordmetric-server aims to be a StatsD+graphite competitor, it implements
a wire compatible StatsD API.

FnordMetric Server can store the collected timeseries data on local disk or in
external storage (HBase).

Documentation
-------------

You can find the full FnordMetric Documentation at http://fnordmetric.io/


Contributions
-------------

#### Individial Contributors to FnordMetric

+ Christian Parpart (http://github.com/trapni)
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

#### Other Open Source Software included in FnordMetric

  + Marijn Haverbeke's CodeMirror --- javascript code editor (https://github.com/marijnh/codemirror)
  + FontAwesome --- icon font (http://fontawesome.io/)

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
