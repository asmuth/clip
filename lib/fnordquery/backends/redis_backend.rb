class FnordQuery::RedisBackend

  def initialize(opts={})
  	@opts = opts
  end

  def subscribe(query, &block)

  end

  def on_finish(&block)

  end

  def publish(event, opts={})
  	puts "publishing event"
  end

end