class FnordMetric::Worker

  # events will be kept in mem for one day
  @@expiration_time = 3600*24

  def initialize(namespaces, opts)        
    @redis = EM::Hiredis.connect("redis://localhost:6379")
    @namespaces = {}
    @opts = opts
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
      @redis.get(event_key(event_id)).callback do |event_data|                     
        process_event(event_id, event_data) if event_data        
        FnordMetric.log("oops, lost an event :(") unless event_data
        EM.next_tick(&method(:tick))      
        @redis.hincrby(stats_key, :events_processed, 1)
      end
    end
  end

  def process_event(event_id, event_data)
    EM.defer do      
      parse_json(event_data).tap do |event|
        publish_event(event_id)
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
    namespace(event[:_namespace]).ready!(@redis).announce(event)
  end

  def expire_event(event_id)
    @redis.expire(event_key(event_id), @@expiration_time)
  end

  def publish_event(event_id)
    @redis.publish(pubsub_key, event_id)
  end

  def namespace(key)
    (@namespaces[key] || @namespaces.first.last).clone
  end

  def parse_json(data)
    Yajl::Parser.new(:symbolize_keys => true).parse(data)
  end

end
