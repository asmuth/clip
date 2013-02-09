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
