FnordMetric
===========

FnordMetric is a highly configurable (and pretty fast) realtime app/event 
tracking thing based on ruby eventmachine and redis. You define your own 
plotting and counting functions as ruby blocks!

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/fnordmetric.png) ](http://travis-ci.org/paulasmuth/fnordmetric)


http://fnordmetric.io/


[ ![Screenshot](https://github.com/paulasmuth/fnordmetric2/raw/v1.0/doc/preview3.png) ](http://fnordmetric.io/)



Getting Started
---------------

Simple Example: this will listen for json-events with `type=unicorn_seen` 
and render a timeline-plot showing the number of received events per hour.

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

  # timeline+plot and punchcard
  timeseries_gauge :unicorns_seen_per_hour, 
    :resolution => 5.minutes,
    :title => "Unicorns seenper Hour",
    :punchcard => true

  # on every event like { _type: 'unicorn_seen' }
  event(:unicorn_seen) do
    # increment the unicorns_seen_per_hour gauge by 1
    incr :unicorns_seen_per_hour
  end

end

FnordMetric.standalone
```

This is the easiest way to submit an event:

    echo "{\"_type\": \"unicorn_seen\"}" | nc localhost 1337



Installation
------------

    gem install fnordmetric

or in your Gemfile:

    gem 'fnordmetric', '>= 0.6.3'


Documentation
-------------

Please visit http://fnordmetric.io/ for the full documentation.


Resources
----------

+ [PHP API for FnordMetric (github.com/leemachin/fnordmetric-php-api)](https://github.com/leemachin/fnordmetric-php-api)
+ [Python API for FnordMetric (github.com/sholiday/pyfnordmetric)](https://github.com/sholiday/pyfnordmetric)
+ [Another Ruby API for FnordMetric](https://github.com/savonarola/fnordmetric-client)
+ [Beanstalk Monitoring with FnordMetric](https://github.com/sholiday/fnordstalk)
+ [Blog: Monitor your Python App With FnordMetric](http://stephenholiday.com/articles/2012/monitor-your-python-app-with-fnordmetric/)
+ [Blog: FnordMetric and C (Blog/Howto)](http://johnmurray.io/log/2012/01/19/FnordMetrics-and-C%23.md)
+ [Blog: FnordMetric HowTo (russian)](http://www.pvsm.ru/ruby/2723)
+ [The "FnordMetric" google group](http://groups.google.com/group/fnordmetric)


Full Example
------------

    here be dragons



Contributors
------------

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