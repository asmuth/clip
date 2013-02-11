Getting Started
---------------


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

The canonical way to submit data is the HTTP + JSON api, this will report a single sale:

    curl -X POST -d '{ "_type": "_incr", "value": 1, "gauge": "sales_per_minute" }' http://localhost:4242/events


<img src="/img/simple_example_screen.png" width="630" style="box-shadow:0 0 8px rgba(0,0,0,.4); />
