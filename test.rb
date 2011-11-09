require "redis"
require "json"

redis = Redis.new

# send 10.000 simple events to fnordmetric

event = { :_type => "foobar" }.to_json

10000.times do |i| 

  my_uuid = i.to_s # generate a unique event id ;)

  redis.lpush("fnordmetric-queue", my_uuid) 
  redis.set("fnordmetric-event-#{my_uuid}", event)

end

redis.keys("fnordmetric-blubber*").each do |k|
  puts "#{k} -> #{redis.get(k)}"
end