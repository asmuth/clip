class FnordMetric::Worker

  def initialize(namespaces, opts)        
    @namespaces = {}
    @opts = opts
    configure(namespaces)
  end

  def ready!
    @async_redis = EM::Hiredis.connect(@opts[:redis_url])
    @sync_redis = Redis.new(:url => @opts[:redis_url])
    tick
  end

  def configure(namespaces)   
    namespaces.each do |key, block|
      @namespaces[key] = FnordMetric::Namespace.new(key, @opts.clone)
      @namespaces[key].instance_eval(&block)
    end
  end

  def tick
    @async_redis.blpop(queue_key, 0).callback do |list, event_id|           
      @async_redis.get(event_key(event_id)).callback do |event_data|                     
        process_event(event_id, event_data) if event_data        
        FnordMetric.log("event_lost: event_data not found for event-id '#{event_id}'") unless event_data
        EM.next_tick(&method(:tick))      
        @async_redis.hincrby(stats_key, :events_processed, 1)
      end
    end
  end

  def process_event(event_id, event_data)
    EM.defer do      
      parse_json(event_data).tap do |event|                
        event[:_time] ||= Time.now.to_i
        event[:_eid] = event_id
        announce_event(event)
        publish_event(event)        
        expire_event(event_id)       
      end
    end
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

  def stats_key
    [@opts[:redis_prefix], 'stats'].join("-")
  end

  def announce_event(event)   
    namespace(event[:_namespace]).ready!(@sync_redis).announce(event)
  end

  def expire_event(event_id)
    @sync_redis.expire(event_key(event_id), @opts[:event_data_ttl])
  end

  def publish_event(event)    
    @sync_redis.publish(pubsub_key, event[:_eid])    
  end

  def namespace(key)
    (@namespaces[key] || @namespaces.first.last).clone
  end

  def parse_json(data)
    event = Yajl::Parser.new(:symbolize_keys => true).parse(data)
    event[:_namespace] = event[:_namespace].to_sym if event[:_namespace]
    event
  end

end
