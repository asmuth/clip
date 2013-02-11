require "fnordmetric"

FnordMetric.namespace :myapp do

=begin
  # render a timeseries graph
  widget 'Sales',
    :title => "Sales per Minute",
    :gauges => [:sales_per_minute],
    :type => :timeline,
    :width => 100,
    :autoupdate => 10
=end

end

FnordMetric.standalone
