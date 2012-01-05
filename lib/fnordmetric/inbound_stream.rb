class FnordMetric::InboundStream < EventMachine::Connection 
  @@opts = nil

  def self.start(opts)
    @@opts = opts
    EM.start_server(*opts[:inbound_stream], self)    
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
    @api.event event_data
    @events_buffered -= 1
    close_connection?
    EM.next_tick(&method(:push_next_event))    
  end

  def close_connection?
    @api.disconnect unless @streaming || (@events_buffered!=0) 
  end

  def post_init
    @api = API.new(@@opts)
    @events_buffered = 0
    @streaming = true
    @buffer = ""
    @events = []          
  end

  def unbind
    @streaming = false
    close_connection?
  end
end
