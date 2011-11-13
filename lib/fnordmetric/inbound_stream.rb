class FnordMetric::InboundStream < EventMachine::Connection 

  @@timeout = 3

  def post_init
     @redis = Redis.new
     @events_buffered = 0
     @streaming = true
     @buffer = ""
     @events = []          
  end

  def get_next_uuid
    rand(9999999999999999999).to_s
  end
  
  def unbind
    @streaming = false
    close_connection?
  end

  def receive_data(chunk)        
    @buffer << chunk         
    EM.defer{ next_event }
  end

  def next_event
    read_next_event
    push_next_event
  end
  
  def read_next_event
    while (event = @buffer.slice!(/^(.*)\n/))
      @events_buffered += 1
      @events << event
    end 
  end

  def push_next_event
    return true if @events.empty?
    push_event(get_next_uuid, @events.pop) 
    EM.next_tick(&method(:push_next_event))    
  end

  def push_event(event_id, event_data)
    @redis.hincrby("fnordmetric-stats", "events_received", 1)
    @redis.set("fnordmetric-event-#{event_id}", event_data)
    @redis.lpush("fnordmetric-queue", event_id)
    @redis.expire("fnordmetric-event-#{event_id}", 60) 
    @events_buffered -= 1
    close_connection?
  end


  def close_connection?
    @redis.quit unless @streaming || (@events_buffered!=0) 
  end

end
