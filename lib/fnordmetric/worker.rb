class FnordMetric::Worker

  @@expiration_time = 10
  @@idle_time = 0.3

  def initialize(opts)
    @opts = opts
    @redis = opts.fetch(:redis)
    loop{ work! }
  end

  def pubsub_key
    [@opts[:redis_prefix], 'announce'].join("-")
  end

  def queue_key
    [@opts[:redis_prefix], 'queue'].join("-")
  end

  def event_key(event_id)
    [@opts[:redis_prefix], 'event', event_id].join("-")
  end

  def try_event(event_id) 
    event_data = @redis.get(event_key(event_id))
    return false unless event_data
    publish_event(event_id)
    process_event(event_data)    
    expire_event(event_id)
  end

  def process_event(event_data)
    JSON.parse(event_data).tap do |event|      
      namespace = FnordMetric.namespace(event["_namespace"])
      namespace.announce(event)
    end
  end

  def expire_event(event_id)
    @redis.expire(event_key(event_id), @@expiration_time)
  end

  def publish_event(event_id)
    @redis.publish(pubsub_key, event_id)
  end

  def work!(_key=queue_key)
    (n=@redis.rpop(_key)) ? try_event(n) : idle!
  end

  def idle!
    sleep(@@idle_time)
  end

end