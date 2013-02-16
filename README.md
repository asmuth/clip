FnordMetric
===========

FnordMetric is a highly configurable (and pretty fast) realtime app/event 
tracking thing based on [ruby eventmachine](http://rubyeventmachine.com/) and [redis](http://redis.io/). You define your own 
plotting and counting functions as ruby blocks! [See it in action! (RailsCasts)](http://railscasts.com/episodes/378-fnordmetric)

Documentation: [fnordmetric.io](http://fnordmetric.io/)

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/fnordmetric.png) ](http://travis-ci.org/paulasmuth/fnordmetric)

[ ![Screenshot](https://github.com/paulasmuth/fnordmetric/raw/v1.0-alpha/doc/preview3.png) ](http://github.com/paulasmuth/fnordmetric)



Getting Started
---------------

FnordMetric is based on ruby eventmachine and needs to run in a seperate ruby process.
The preferred way to start it is to create a ruby file (where you put your DSL statements)
and execute it (more about that in [the documentation](http://fnordmetric.io/documentation))

Save this to `my_fnordmetric.rb`

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

  # render a timeseries graph
  widget 'Sales',
    :title => "Sales per Minute",
    :gauges => [:sales_per_minute],
    :type => :timeline,
    :width => 100,
    :autoupdate => 1

end

FnordMetric.standalone
```

In this case we created one timeseries chart on the dashboard "Sales" that will display
the number of sales_per_minute and auto-refresh every second.

You should now be able to start the dashboard on http://localhost:4242/ (default) by running:

    $ ruby my_fnordmetric.rb


Now we can start sending data to FnordMetric. The canonical way to submit data is the HTTP API.
This will report a single sale:

    curl -X POST -d '{ "_type": "_incr", "value": 1, "gauge": "sales_per_minute" }' http://localhost:4242/events

There are various other ways to submit events to FnordMetric (more information in [the documentation](http://fnordmetric.io/documentation)).


### HTML5 API

FnordMetric offers a HTML5 / JavaScript API (called _FnordMetric UI_) that allows
you to plug real-time data and charts into any website without having to write code.
This is achieved by including a JavaScript library and using data-* attributes on
html elements to declare the widgets.

```html
<link href='http://localhost:4242/fnordmetric-ui.css' type='text/css' rel='stylesheet' />
<script src='http://localhost:4242/fnordmetric-ui.js' type='text/javascript'></script>

<span
  data-fnordmetric="counter"
  data-gauge="sales_per_minute"
  data-autoupdate="1"
  data-unit="€"
  >0</span>

<script>
  FnordMetric.setup({
    "address":   "localhost:4242",
    "namespace": "myapp"
  });
</script>
```


Installation
------------

    gem install fnordmetric

or in your Gemfile:

    gem 'fnordmetric', '>= 1.0.0'


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
