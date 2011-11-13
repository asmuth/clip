class FnordMetric::Session

  def self.create(opts)
    @opts = opts    
    
    redis = @opts.fetch(:redis)
    event = @opts[:event]

    session_key_hash = Digest::MD5.hexdigest(event[:_session])

    sessions_set_key = [@opts[:namespace_prefix], 'sessions'].join("-")
    session_events_key = [sessions_set_key, session_key_hash, 'events'].join("-")

    redis.rpush(session_events_key, event[:_eid] )
    redis.zadd(sessions_set_key, event[:_time], session_key_hash)
  end

  def self.find(session_key)
    self.new(session_key).tap do |session|
      #session.fetch_data!
      #session.fetch_event_ids!
      #session.fetch_events!
    end
  end

  def self.all(since=nil)
    []
  end

  def picture
    "sdgjsdfg"
  end

  def name
    "areazrh"
  end

  def data(key)
    {}
  end

  def event_ids
    []
  end
  
  def events
    [] 
  end

end