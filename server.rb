$: << ::File.expand_path("../lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  gauge :blubbers, :tick => 10

  event :foobar do
    incr(:blubbers, 1)
    puts "fuuu: #{data} ! #{key(:blubbers)}"
  end

end

FnordMetric.run 
