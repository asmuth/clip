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

Simple Example: this will listen for json-events with `type=unicorn_seen` 
and render a timeline-plot showing the number of received events per minute.

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

  # a gauge is basically a counter 
  gauge :unicorns_seen,
    :tick => 1.minute.to_i,
    :title => "Unicorns per Minute" 

  # render a timeseries graph
  widget 'Unicorns',
    :title => "Unicorns per Minute",
    :gauges => [:unicorns_seen],
    :type => :timeline,
    :width => 100,
    :autoupdate => 10

  # this code is called for every incoming event with type = 'unicorn_seen'
  event(:unicorn_seen) do
    # increment the unicorns_seen_per_hour gauge by 1
    incr :unicorns_seen
  end

end

FnordMetric.standalone
```

This is the easiest way to submit an event is using `netcat` from the commandline:

    echo '{"_type": "unicorn_seen"}' | nc localhost 1337


FnordMetric offers a HTML5 / JavaScript API to plug the realtime data into any webpage:

```html
<link href='http://localhost:4242/fnordmetric-ui.css' type='text/css' rel='stylesheet' />
<script src='http://localhost:4242/fnordmetric-ui.js' type='text/javascript'></script>

<span
  data-fnordmetric="counter"
  data-gauge="unicorns_seen"
  data-autoupdate="1"
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

+ [Blog: Monitor your Python App With FnordMetric](http://stephenholiday.com/articles/2012/monitor-your-python-app-with-fnordmetric/)
+ [Blog: FnordMetric and C (Blog/Howto)](http://johnmurray.io/log/2012/01/19/FnordMetrics-and-C%23.md)
+ [Blog: FnordMetric HowTo (russian)](http://www.pvsm.ru/ruby/2723)
+ [RailsCast: FnordMetric (378)](http://railscasts.com/episodes/378-fnordmetric)


### More Resources

+ [PHP API for FnordMetric (use TCP) (github.com/leemachin/fnordmetric-php-api)](https://github.com/leemachin/fnordmetric-php-api)
+ [PHP API for FnordMetric directly into Redis (github.com/votintsev/fnordmetric-php-redis)](https://github.com/votintsev/fnordmetric-php-redis)
+ [Python API for FnordMetric (github.com/sholiday/pyfnordmetric)](https://github.com/sholiday/pyfnordmetric)
+ [Another Ruby API for FnordMetric](https://github.com/savonarola/fnordmetric-client)
+ [Beanstalk Monitoring with FnordMetric](https://github.com/sholiday/fnordstalk)
+ [The "FnordMetric" google group](http://groups.google.com/group/fnordmetric)


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
