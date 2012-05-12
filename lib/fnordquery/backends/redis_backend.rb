class FnordQuery::RedisBackend

  def initialize(opts={})
  	@opts = opts
  end

  def subscribe(opts, &block)

  end

  def publish(event, opts={})
  	puts "publishing event"
  end

end