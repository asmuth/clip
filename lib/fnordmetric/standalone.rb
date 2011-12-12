require 'rake'
require 'redis'

task :run do  
  FnordMetric.run
end

task :log do
  FnordMetric::Logger.start(dump_file_path)
end

task :import do
  FnordMetric::Logger.import(dump_file_path)
end

task :help do
  puts "usage: #{$0} {run|log|import} [DUMP_FILE=fm_dump.json]"
end

task :default => :help

def dump_file_path
  if ENV["DUMP_FILE"].blank?
  	Rake::Task[:help].execute; exit!
  else
  	::File.expand_path(ENV["DUMP_FILE"], ::File.dirname($0))
  end
end

Rake.application.init('fnordmetric')
Rake.application.top_level