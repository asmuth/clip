require 'rake'

task :help do
  print_version
  print_help
  print_examples
  print_copy
end

task :default => :help

def print_version
  puts "fnordquery 0.0.1\n\n"
end

def print_help
  help = <<-EOH
  Usage: fnordquery [OPTIONS...] [TASK]
  -r <address>     use redis backend
  -x <address>     use fyrehose backend
  -T               print all tasks
  -h               print this message
  EOH
  puts [help.lstrip, nil]
end

def print_examples
  examples = <<-EOH
  Examples:
  fnordquery -r localhost:6379 web[0.0.0.0:8080]
  fnordquery -r localhost:6379 listen:tcp[0.0.0.0:2325]
  fnordquery -r localhost:6379 listen:udp[0.0.0.0:2323]
  EOH
  puts [examples.lstrip, nil]
end

def print_copy
  puts "http://github.com/paulasmuth/fnordquery"
end

Rake.application.init('fnordquery')
Rake.application.top_level