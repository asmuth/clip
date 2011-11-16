$: << ::File.expand_path("../../fnordmetric/lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  widget 'Channels', nil

end

fm_opts = {}
fm_opts.merge!(:web_interface => ["0.0.0.0", "2323"]) if ENV['DEV']

FnordMetric.run(fm_opts)
