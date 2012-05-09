class FnordMetric::RedisBackend

  def initialize(opts)
    @callback = nil
    @redis_channel = opts[:redis_prefix]

  	@sub_redis = EM::Hiredis.connect(opts[:redis_url])
    @pub_redis = EM::Hiredis.connect(opts[:redis_url])

    @sub_redis.subscribe(@redis_channel)
  end

  def subscribe(&block)
    @sub_redis.on(:message) do |chan, message|
      block.call(message)
    end
  end

  def publish(message)
    @pub_redis.publish(@redis_channel, message.to_json)
  end

  def hangup
    @pub_redis.close
    @sub_redis.close
  end

end