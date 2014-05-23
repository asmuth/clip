class FnordMetric::Worker

  def initialize(opts = {})
    @namespaces = FnordMetric.namespaces
    @opts = FnordMetric.options(opts)

    FnordMetric.register(self)
  end

  def initialized
    FnordMetric.log("worker started")
    EM.next_tick(&method(:tick))
  end

  def tick
    redis.blpop(queue_key, 1).callback do |list, event_id|
      EM.next_tick(&method(:tick))
      if event_id
        redis.get(event_key(event_id)).callback do |event_data|
          process_event(event_id, event_data) if event_data
          FnordMetric.log("event_lost: event_data not found for event-id '#{event_id}' - maybe expired?") unless event_data
          redis.hincrby(stats_key, :events_processed, 1)
        end
      end
    end
  end

  def process_event(event_id, event_data)
    EM.next_tick do
      event = parse_json(event_data)
      if event
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
    namespace(event[:_namespace]).ready!(redis, sync_redis).announce(event)
  end

  def expire_event(event_id)
    redis.expire(event_key(event_id), @opts[:event_data_ttl])
  end

  def publish_event(event)    
    redis.publish(pubsub_key, event.to_json)
  end

  def namespace(key)
    (@namespaces[key] || @namespaces.first.last).clone
  end

  def parse_json(data)
    event = Yajl::Parser.new(:symbolize_keys => true).parse(data)
    event[:_namespace] = event[:_namespace].to_sym if event[:_namespace]
    event
  rescue Yajl::ParseError => e
    FnordMetric.error "invalid json: #{e.to_s}"; false
  end

  def redis
    @redis ||= EM::Hiredis.connect(FnordMetric.options[:redis_url]) # FIXPAUL
  end

  def sync_redis
    @sync_redis ||= FnordMetric.mk_redis
  end

end
