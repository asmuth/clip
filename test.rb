require "redis"
require "json"

redis = Redis.new
event = { :_type => "foobar" }.to_json

loop do

  # send 1.000 simple events to fnordmetric
    
  1000.times do |i| 

    my_uuid = rand(9999999999999999999).to_s # generate a unique event id ;)

    redis.lpush("fnordmetric-queue", my_uuid) 
    redis.set("fnordmetric-event-#{my_uuid}", event)

  end

  sleep 5

  # see what that did

  redis.keys("fnordmetric-blubber*").each do |k|
    begin
      puts "#{k} -> #{redis.get(k)}"
    rescue
      redis.hgetall(k).each do |_k,v|
        puts "#{k} -> #{_k} -> #{v}"
      end
    end
  end

  puts "\n#{"-"*30}\n\n"

end