require 'rake'

task :run do  
  FnordMetric.run
end

task :help do
  puts "usage: #{$0} {run|log|import} [dump_file]"
end

task :default => :help

Rake.application.init('fnordmetric')
Rake.application.top_level