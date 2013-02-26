FnordMetric
===========

FnordMetric is a framework for collecting and visualizing timeseries data. It enables
you to build beautiful real-time analytics dashboards within minutes.

Documentation: [fnordmetric.io](http://fnordmetric.io/)

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/fnordmetric.png) ](http://travis-ci.org/paulasmuth/fnordmetric)

[ ![Screenshot](https://github.com/paulasmuth/fnordmetric/raw/v1.0-alpha/doc/preview3.png) ](http://github.com/paulasmuth/fnordmetric)


### FnordMetric Enterprise

FnordMetric Enterprise is a JVM-based timeseries database. It's a key-value store
(much like redis or memcached) where each key holds a "measurement". There are
different measurement methods like sum, mean, min/max, 90th percentile, etc. You
continuously add data to these keys/measurements which is aggregated and periodically
persisted.

FnordMetric Enterprise features disk persistence, a HTTP, TCP and UDP API, native
WebSockets support, CSV/JSON Export and a turnkey-ready HTML5 visualization solution
(FnordMetric UI). FnordMetric Enterprise can be used as a drop-in replacement for
StatsD+Graphite (it is a lot faster, see Benchmarks).

[Getting Started with FnordMetric Enterprise](http://fnordmetric.io/documentation/enterprise_index)


### FnordMetric UI

FnordMetric UI is a HTML5 API that lets you plug realtime data and charts into any webpage
without writing a single line of code. It gives you maximum flexiblitiy as you have full
control over layout and style with HTML and CSS.

FnordMetric UI uses WebSockets to communicate with a backend server. There are two backend
implementations: FnordMetric Classic (ruby + redis) and FnordMetric Enterprise (JVM). You can use
FnordMetric UI as a white label solution to power your custom realtime analytics apps.

[Getting Started with FnordMetric UI](http://fnordmetric.io/documentation/ui_index)


### FnordMetric Classic

FnordMetric Classic is powered by ruby and redis. It offers a ruby DSL for processing data
streams and building beautiful web dashboards from a collection of turnkey-ready UI widgets.
You define your own plotting and aggregation methods as ruby blocks.  [See it in action! (RailsCasts)](http://railscasts.com/episodes/378-fnordmetric)

[Getting Started with FnordMetric Classic](http://fnordmetric.io/documentation/classic_index)



Documentation
-------------

You can find the full FnordMetric Documentation at http://fnordmetric.io/



Contributors
------------

http://github.com/paulasmuth/fnordmetric/graphs/contributors

(One patch or more)

+ Simon Menke (http://github.com/fd)
+ Bruno Michel (http://github.com/nono)
+ Marco Borromeo (http://github.com/mborromeo)
+ Leo Lou (http://github.com/l4u)
+ Andy Lindeman (http://github.com/alindeman)
+ Jurriaan Pruis (http://github.com/jurriaan)
+ Kacper Bielecki (http://github.com/kazjote)
+ John Murray (http://github.com/JohnMurray)
+ Lars Gierth (http://github.com/lgierth)
+ Ross Kaffenberger (http://github.com/rossta)
+ Kunal Modi (http://github.com/kunalmodi)
+ Michael Fairchild (http://github.com/fairchild)
+ James Cox (http://github.com/imajes)
+ Pieter Noordhuis (http://github.com/pietern)
+ Tadas Ščerbinskas (http://github.com/tadassce)
+ Sebastian Korfmann (http://github.com/skorfmann)

To contribute, please fork this repository, make your changes and run the 
specs, commit them to your github repository and send me a pull request.
Need help, head on over to our [Google Groups][1]  page to discuss any ideas
that you might have.


License
-------

Copyright (c) 2011 Paul Asmuth

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to use, copy and modify copies of the Software, subject 
to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  [1]: http://groups.google.com/group/fnordmetric
  [2]: http://www.screenr.com/KiJs
  [3]: https://secure.travis-ci.org/paulasmuth/fnordmetric.png
  [4]: http://travis-ci.org/paulasmuth/fnordmetric
