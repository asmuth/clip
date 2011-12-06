require "rubygems"
require "redis"
require "json"

if ARGV.length != 1
  puts "usage: #{$0} /path/to/dump.json"
  exit!
end

file_path = ARGV[0] 

puts "opening #{file_path}"
file = File.open(file_path)

puts "reading file..."
events = file.read.split("\n")

puts "pushing #{events.length} events to redis"
redis = Redis.new
total_events = events.length
events.each_with_index do |event,n|
  my_uuid = rand(9999999999999999999).to_s # generate a unique event id ;)
  redis.lpush("fnordmetric-queue", my_uuid) 
  redis.set("fnordmetric-event-#{my_uuid}", event)
  puts "#{n}/#{total_events}" if (n%100==0)
end
