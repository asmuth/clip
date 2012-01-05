class FnordMetric::InboundStream < EventMachine::Connection
  module Logic
    def self.opts=(opts)
      @@opts = opts
    end

    @@em = EM
    def self.em=(em)
      @@em = em
    end

    def receive_data(chunk)
      @buffer << chunk
      @@em.defer{ next_event }
    end

    def push_event(event_id, event_data)    
      prefix = @@opts[:redis_prefix]

      JSON.parse(event_data)
      @redis.hincrby "#{prefix}-stats",             "events_received", 1
      @redis.set     "#{prefix}-event-#{event_id}", event_data
      @redis.lpush   "#{prefix}-queue",             event_id       
      @redis.expire  "#{prefix}-event-#{event_id}", @@opts[:event_queue_ttl]
    rescue JSON::ParserError
    ensure
      @events_buffered -= 1
      close_connection?
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
      @@em.next_tick(&method(:push_next_event))    
    end

    def get_next_uuid
      rand(9999999999999999999).to_s # FIXME
    end

    def close_connection?
      @redis.quit unless @streaming || (@events_buffered!=0) 
    end

    def post_init
      @redis = Redis.connect(:url => @@opts[:redis_url])
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

  Logic.opts = nil

  include Logic

  def self.start(opts)
    Logic.opts = opts
    EM.start_server(*opts[:inbound_stream], self)
  end
end

