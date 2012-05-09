class FnordMetric::Session
 
  attr_accessor :updated_at, :name, :picture

  @@meta_attributes = %w(name picture)

  def self.create(opts)        
    redis = opts.fetch(:redis)
    event = opts[:event]   

    hash = Digest::MD5.hexdigest(event[:_session])
    set_key = "#{opts[:namespace_prefix]}-session"

    self.new(hash).tap do |session|
      session.add_redis(redis, set_key)      
      session.add_event(event)    
      session.expire(opts[:session_data_ttl])
    end    
  end

  def self.find(session_key, opts)
    set_key = "#{opts[:namespace_prefix]}-session"
    self.new(session_key, [opts[:redis], set_key])
  end

  def self.all(opts)    
    set_key = "#{opts[:namespace_prefix]}-session"
    limit = (opts[:limit].try(:to_i)||0)-1
    session_ids = opts[:redis].zrevrange(set_key, 0, limit, :withscores => true)
    session_ids.in_groups_of(2).map do |session_key, ts|
      find(session_key, opts).tap{ |s| s.updated_at = ts }
    end
  end

  def initialize(session_key, redis_opts=nil)
    @session_key = session_key
    add_redis(*redis_opts) if redis_opts
  end

  def session_key
    @session_key
  end

  def picture
    @picture
  end

  def name
    @name
  end

  def data(key=nil)
    key ? @data[key] : @data 
  end

  def event_ids
    @event_ids || []
  end

  def events
    [] 
  end

  def to_json
    { :session_key => session_key }.tap do |hash| 
      hash.merge!(:_picture => @picture) if @picture
      hash.merge!(:_name => @name) if @name
      hash.merge!(:_updated_at => @updated_at) if @updated_at
    end
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
    @redis.zadd(redis_key(:events), event[:_time], event[:_eid])    

    add_data(:_picture, event[:url]) if event[:_type] == "_set_picture"    
    add_data(:_name, event[:name]) if event[:_type] == "_set_name"    
    add_event_data(event) if event[:_type] == "_set_data" 
    touch(event[:_time])
  end

  def add_event_data(event)
    event.each do |key, value|
      add_data(key, value) unless key.to_s.starts_with?("_")
    end
  end

  def add_data(key, value)    
    @redis.hset(redis_key(:data), key, value)
  end
  
  def fetch_data!
    @data = Hash.new
    @redis.hgetall(redis_key(:data)).each do |key, value|    
      if key.to_s.starts_with?("_")
        fetch_meta_key(key, value)
      else      
        @data[key.intern] = value 
      end
    end
  end

  def fetch_meta_key(key, value)
    meta_key = key[1..-1]
    if @@meta_attributes.include?(meta_key)
      instance_variable_set(:"@#{meta_key}", value)
    end
  end

  def fetch_event_ids!(since=-1)
    # FIXME: use WITHSCORE to get the timestamps and return event objects
    @event_ids = @redis.zrevrange(redis_key(:events), 0, since)
  end

end
