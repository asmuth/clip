class FnordMetric::InboundStream  < EventMachine::Connection 

  def post_init
     @redis = EM::Hiredis.connect("redis://localhost:6379")
     @parser = Yajl::Parser.new
     @parser.on_parse_complete = method(:process_event)
     @buffer = ""
  end

  def process_event(event)
    @redis.hincrby("fnordmetric-stats", "events_received", 1)    
    event_data = Yajl::Encoder.encode(event)
    EM.defer{ push_event(event_data) }
  end

  def push_event(event_data)
    my_uuid = rand(9999999999999999999).to_s # FIXME 
    @redis.set("fnordmetric-event-#{my_uuid}", event_data).callback do
      @redis.lpush("fnordmetric-queue", my_uuid)
      # events that aren't processed within 60 seconds get dropped
      @redis.expire("fnordmetric-event-#{my_uuid}", 60)      
    end
  end

  def receive_data(chunk)        
    @parser << chunk         
  end

end
