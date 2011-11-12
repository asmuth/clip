class FnordMetric::Worker

  @@expiration_time = 10
  @@idle_time = 0.3

  def initialize(namespaces, opts)    
    @namespaces = {}
    @opts = opts    
    @parser = Yajl::Parser.new
    @parser.on_parse_complete = method(:process_event)
    @redis = EM::Hiredis.connect("redis://localhost:6379")
    configure(namespaces)
    tick
  end

  def configure(namespaces)   
    namespaces.each do |key, block|
      @namespaces[key] = FnordMetric::Namespace.new(key, @opts.clone)
      @namespaces[key].instance_eval(&block)
    end
  end

  def tick
    @redis.blpop('fnordmetric-queue', 0).callback do |list, event_id|      
      EM.next_tick(&method(:tick)) 
      @redis.get(event_key(event_id)).callback do |event_data|                  
        push_event(event_id, event_data) if event_data        
        @redis.hincrby(stats_key, :events_processed, 1)
      end
    end
  end

  def namespace(key)
    (@namespaces[key] || @namespaces.first.last)
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

  def push_event(event_id, event_data)            
    publish_event(event_id)
    @parser << event_data
    expire_event(event_id)
  end

  def process_event(event)
    EM.defer do
      event.merge!(:time => Time.now.getutc.to_i)
      namespace(event["_namespace"]).clone.ready!.announce(event)          
    end
  end

  def expire_event(event_id)
    @redis.expire(event_key(event_id), @@expiration_time)
  end

  def publish_event(event_id)
    @redis.publish(pubsub_key, event_id)
  end

end
