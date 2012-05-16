module FnordMetric::AppHelpers

  def h(*args)
    escape_html(*args)
  end
  
  def path_prefix
    request.env["SCRIPT_NAME"]
  end

  def namespaces
    @namespaces
  end

  def current_namespace
    @namespaces[@namespaces.keys.detect{ |k|
      k.to_s == params[:namespace]
    }.try(:intern)]
  end

  def parse_params(hash)
    hash.tap do |h|
      h.keys.each{ |k| h[k] = parse_param(h[k]) }
    end
  end

  def parse_param(object)
    return object unless object.is_a?(String)
    return object.to_f if object.match(/^[0-9]+[,\.][0-9]+$/)
    return object.to_i if object.match(/^[0-9]+$/)
    object
  end

  def track_event(event_id, event_data)
    # FIXPAUL: use api
    @redis.hincrby "#{@opts[:redis_prefix]}-stats",             "events_received", 1
    @redis.set     "#{@opts[:redis_prefix]}-event-#{event_id}", event_data.to_json
    @redis.lpush   "#{@opts[:redis_prefix]}-queue",             event_id
    @redis.expire  "#{@opts[:redis_prefix]}-event-#{event_id}", @opts[:event_queue_ttl]
  end

end