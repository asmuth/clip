Getting Started
---------------

The basic concept in FnordMetric is called a "gauge". A Gauge is a primitive data store for a numerical value over
time. A gauge has two dimensions: Value und Time. Gauges can be used in different modes like sum,
average, max/min, etcetara.

The FnordMetric core deals with processing data. There are two basic concepts:
events and gauges.

An Event is a piece of data that is sent to FnordMetric through one of the various
sources. This event is a JSON Object / Hash with almost no constraints on the schema.

You can install FnordMetric Classic by runing

    gem install fnordmetric


Simple Example: this will render a timeline-plot showing the number of sales per minute.

    require "fnordmetric"

    FnordMetric.namespace :myapp do

      # render a timeseries graph
      widget 'Sales',
        :title => "Sales per Minute",
        :gauges => [:sales_per_minute],
        :type => :timeline,
        :width => 100,
        :autoupdate => 10

    end

    FnordMetric.standalone

The easiest way to submit an event (i.e. a piece of data) is using `netcat` from the commandline:

    echo '{ "_type": "_incr", "value": 1, "gauge": "sales_per_minute", "flush_interval": 60 }' | nc localhost 1337

or you can use the HTTP API:

    curl -X POST -d '{ "_type": "_incr", "value": 1, "gauge": "sales_per_minute", "flush_interval": 60 }' http://localhost:4242/events


### Gauges

here be dragons


### Event Handlers

You write event handlers in ruby that get invoked per incoming event and modify (increment,
set, etcetera) gauges.
