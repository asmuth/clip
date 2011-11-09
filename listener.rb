require "redis"
require "json"

event_ids = Queue.new

fetcher = Thread.new do
  redis = Redis.new
  loop do

    event_id = event_ids.pop
	event_data = redis.get("fnordmetric-event-#{event_id}")
	event = JSON.parse(event_data)
	
	#event = event_data

    print "\033[1;34m" 
	print event.inspect
	print "\033[0m"  	
  	print "\n"

  	STDOUT.flush

  end
end

listener = Thread.new do
  redis = Redis.new
  redis.subscribe("fnordmetric-announce") do |on|

    on.message do |channel, event_id|
      event_ids << event_id
    end

  end
end

fetcher.join