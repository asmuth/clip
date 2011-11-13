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

end