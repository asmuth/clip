class FnordMetric::API
  @@opts = nil

  def initialize opts
    @@opts = FnordMetric.default_options(opts)
    connect
  end

  def connect
    @redis = Redis.connect(:url => @@opts[:redis_url])
  end

  def event(event_data)
    begin
      if event_data.is_a?(Hash)
        event_data = event_data.to_json
      else
        JSON.parse(event_data) # void ;)
      end
    rescue JSON::ParserError
      FnordMetric.log("event_lost: can't parse json")
    else
      push_event(get_next_uuid, event_data)
    end
  end

  def disconnect
    @redis.quit
  end

  private

  def push_event(event_id, event_data)
    prefix = @@opts[:redis_prefix]
    @redis.hincrby "#{prefix}-testdata",          "events_received", 1
    @redis.hincrby "#{prefix}-stats",             "events_received", 1
    @redis.set     "#{prefix}-event-#{event_id}", event_data
    @redis.lpush   "#{prefix}-queue",             event_id
    @redis.expire  "#{prefix}-event-#{event_id}", @@opts[:event_queue_ttl]
    event_id
  end

  def get_next_uuid
    rand(8**32).to_s(36)
  end
end
