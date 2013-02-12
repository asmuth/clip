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

FnordMetric.options = {
  :inbound_stream => ["0.0.0.0",2323],
  :inbound_protocol => :tcp
}

FnordMetric.standalone

