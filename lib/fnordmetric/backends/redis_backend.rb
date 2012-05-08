class FnordMetric::RedisBackend

  def initialize(opts)
  	sub_redis = EM::Hiredis.connect(opts[:redis_url])
    pub_redis = EM::Hiredis.connect(opts[:redis_url])

    sub_redis.subscribe(opts[:redis_prefix])

    @chan_feed     = opts[:chan_feed]
    @chan_upstream = opts[:chan_upstream]

    sub_redis.on(:message) do |channel, message|
      @chan_feed.push(JSON.parse(message)) 
    end

    @chan_upstream.subscribe do |message|
      pub_redis.publish(opts[:redis_prefix], message.to_json)
    end
  end

end