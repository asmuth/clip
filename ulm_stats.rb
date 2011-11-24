$: << ::File.expand_path("../../fnordmetric/lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  gauge :events_total, :tick => 10

  event :foobar do
    incr :events_total
  end


  widget 'Channels', {
    :title => "Snafu Foobar",
    :type => :timeline,
    :width => 30,
    :gauges => :events_total
  }

  widget 'Channels', {
    :title => "Blubb Yeah",
    :type => :timeline,
    :width => 70,
    :gauges => :events_total
  }

  widget 'Channels', {
    :title => "Events (total)",
    :type => :timeline,
    :include_current => false,
    :gauges => :events_total
  }

end

fm_opts = {}
fm_opts.merge!(:web_interface => ["0.0.0.0", "2323"]) if ENV['DEV']

FnordMetric.run(fm_opts)
