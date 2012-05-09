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
    @pub_redis.publish(@redis_channel, message.to_json)
  end

  def hangup
    @redis.close
  end

end