class FnordMetric::RedisBackend

  def initialize(opts)
    @redis_channel = opts[:redis_prefix]
    @redis = EM::Hiredis.connect(opts[:redis_url])
    @pub_redis = EM::Hiredis.connect(opts[:redis_url])

    @redis.subscribe(@redis_channel)
  end

  def subscribe(&block)
    @redis.on(:message) do |chan, raw|
      begin
        message = JSON.parse(raw)
      rescue
        puts "redisbackend: received invalid json"
      else
        block.call(message)
      end
    end
  end

  def publish(message)
    out = message.is_a?(String) ? message : message.to_json
    @pub_redis.publish(@redis_channel, out)
  end

  def hangup
    @redis.close
  end

end