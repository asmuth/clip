require "redis"

redis = Redis.new

# send 10.000 simple events to fnordmetric

10000.times do |i| 

  my_uuid = i.to_s # generate a unique event id ;)

  redis.lpush("fnordmetric-queue", my_uuid) 
  redis.set("fnordmetric-event-#{my_uuid}", "{'foo':'bar'}")

end

