class FnordMetric::RedisBackend

  def initialize(opts)
  	sub_redis = EM::Hiredis.connect(opts[:redis_url])
    pub_redis = EM::Hiredis.connect(opts[:redis_url])

    sub_redis.subscribe(opts[:redis_prefix])

    sub_redis.on(:message) do |channel, message|
      FnordMetric.chan_feed.push(JSON.parse(message)) 
    end

    FnordMetric.chan_upstream.subscribe do |message|
      pub_redis.publish(opts[:redis_prefix], message.to_json)
    end
  end

end