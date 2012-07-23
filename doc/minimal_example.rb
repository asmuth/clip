$: << ::File.expand_path("../../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.namespace :fn do

  timeseries_gauge :unicorns_seen,
    :tick => 30.seconds,
    :calculate => :average,
    :series => [:num_unicorns]


  event :unicorn_seen do
    incr :unicorns_seen, :num_unicorns, rand(5)
  end

end

Thread.new do 
  api = FnordMetric::API.new
  loop do
    api.event(:_type => :unicorn_seen)
    sleep rand(10)/10.to_f
  end
end

FnordMetric.standalone
