$: << ::File.expand_path("../lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  gauge :blubbers_delta, :tick => 10
  gauge :blubbers_total, :tick => 10, :progressive => true

  event :foobar do
    #incr(:blubbers_total, 1)
    incr(:blubbers_delta, 1)
    puts "fuuu: #{data} ! #{key(:blubbers_total)}"
  end

end

FnordMetric.run 
