Getting Started
---------------

This will guide you through setting up a simple dashboard that displays the number of
sales per minute. It assumes that you have ruby and a redis server on port 6379 running
on your machine.

We start by installing FnordMetric through rubygems:

    gem install fnordmetric

FnordMetric is based on ruby eventmachine and needs to run in a separate ruby process.
The preferred way to start it is to create a ruby file (where you put your DSL statements)
and execute it (more about that in [Running FnordMetric](/documentation/classic_running_fm))

Save this to a file `my_fnordmetric.rb`

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

This defines a FnordMetric namespace "myapp". You can think of the namespace as the whole
dashboard as you will probably use only one namespace. Namespaces can be used to separate
users if you are deploying FnordMetric as a white-label solution to your customers.

Everything that is inside the block passed to `FnordMetric#myapp` is your custom configuration.
This is where all DSL statements go.

In this case we created one timeseries chart on the dashboard "Sales" that will display
the number of `sales\_per\_minute` and auto-refresh every second.

You should now be able to start the dashboard on http://localhost:4242/ (default) by running:

    $ ruby my_fnordmetric.rb


Now we can start sending data to FnordMetric. In FnordMetric, a piece of input data is called
"event". An event is a JSON object (an arbitrary hashmap). The canonical way to submit events is
the HTTP API. This will report a single sale:

    curl -X POST -d '{ "_type": "_incr", "value": 1, "gauge": "sales_per_minute" }' http://localhost:4242/events

There are various other ways to submit events to FnordMetric (more information in [Sending Data](/documentation/classic_sending_data)).

The "_incr" event we use is a special predefined event that creates a gauge (if it doesn't already exist) and increments it. (You can also define custom events, more about that in [Events and Gauges](/documentation/classic_event_handlers)).

A "gauge" in FnordMetric is basically a bucket that stores a numerical value. It has has two
dimensions: time and value. Each gauge is identified by a uniqe key (here: "sales\_per\_minute").

The value of a gauge is periodically aggregated and persisted into redis. Since we didn't explicitly
define the aggregation interval (`flush\_interval`) the default of 10s will be used for our
`sales\_per\_minute` gauge.

Gauges can be used in different modes: They can act as simple counters with an increment and
a decrement operation, but you can also use them to record the mean / average or the max/min
value (more about that in [Events and Gauges](/documentation/classic_event_handlers)).


If you navigate to http://localhost:4242/ you should see this:

<img src="/img/simple_example_screen.png" width="630" class="shadow" />


You now have a running FnordMetric application. There is a lot more you can do with FnordMetric:

  + Check out the [Full Ruby DSL Example](/documentation/examples/fm_classic_full_example)
  + [List of UI Widgets](/documentation/classic_widgets)
  + Read more about [Events and Gauges](/documentation/classic_event_handlers)
