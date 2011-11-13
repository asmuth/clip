class FnordMetric::Session

  def self.create(opts)        
    redis = opts.fetch(:redis)
    event = opts[:event]   

    hash = Digest::MD5.hexdigest(event[:_session])
    set_key = "#{opts[:namespace_prefix]}-sessions"

    self.new(hash).tap do |session|
      session.add_redis(redis, set_key)      
      session.add_event(event)    
      session.expire(opts[:session_data_ttl])
    end    
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

  def initialize(session_key, redis_opts=nil)
    @session_key = session_key
    add_redis(*redis_opts) if redis_opts
  end

  def redis_key(append=nil)
    [@redis_prefix, @session_key, append].compact.join("-")
  end

  def add_redis(redis, prefix)
    @redis_prefix = prefix
    @redis = redis
  end

  def touch(time=Time.now.to_i)      
    @redis.zadd(@redis_prefix, time, @session_key)
  end

  def expire(time)
    @redis.expire(redis_key(:events), time)
    @redis.expire(redis_key(:data), time)
  end

  def add_event(event)    
    @redis.rpush(redis_key(:events), event[:_eid])
    add_data(:_picture, event[:url]) if event[:_type] == "_set_picture"    
    add_data(:_name, event[:name]) if event[:_type] == "_set_name"    
    touch(event[:_time])
  end

  def add_data(key, value)
    @redis.hset(redis_key(:data), key, value)
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