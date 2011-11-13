class FnordMetric::InboundStream < EventMachine::Connection 

  def post_init
     @redis = EM::Hiredis.connect("redis://localhost:6379")
     @streaming = true
     @buffer = ""
     @events = []
  end

  def get_next_uuid
    rand(9999999999999999999).to_s
  end
  
  def unbind
    @streaming = false
  end

  def receive_data(chunk)        
    @buffer << chunk         
    EM.defer{ read_next_event }
  end
  
  def read_next_event
    while (event = @buffer.slice!(/^(.*)\n/))
      @events << event
    end 
    EM.next_tick(&method(:push_next_event))
  end

  def push_next_event
    return hacky_close_connection? if @events.empty?
    push_event(get_next_uuid, @events.pop)    
  end

  def push_event(event_id, event_data)
    @redis.hincrby("fnordmetric-stats", "events_received", 1) do
      @redis.set("fnordmetric-event-#{event_id}", event_data) do
        @redis.lpush("fnordmetric-queue", event_id) do
          @redis.expire("fnordmetric-event-#{event_id}", 60) do
            EM.next_tick(&method(:push_next_event))     
          end
        end
      end
    end
  end

  def hacky_close_connection?
    return true if @streaming
    @redis.instance_variable_get(:@connection).tap do |c|
      # this is uber-hacky but it works...
      c.close_connection
    end
  end

end
