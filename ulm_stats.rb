$: << ::File.expand_path("../../fnordmetric/lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  widget 'Channels', nil

end

FnordMetric.run 
