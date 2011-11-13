$: << ::File.expand_path("../lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  gauge :blubbers_delta, :tick => 60
  gauge :blubbers_total, :tick => 10, :progressive => true

  event :foobar do
    1.times do # simulate load
      incr(:blubbers_total, 1)
      incr(:blubbers_delta, 1) 
    end
    #puts "fuuu: #{data} ! #{key(:blubbers_total)}"
  end

end

FnordMetric.run 
