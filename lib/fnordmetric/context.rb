class FnordMetric::Context

  def initialize(opts, block)
    @block = block
  	@opts = opts    
    @redis = opts.fetch(:redis)
  end

  def call(event)
  	@event = event    
  	self.instance_eval(&@block)
  end

  private

  def data
  	@event
  end

  def incr(gauge_name, value=1)
    @redis.incrby(key(gauge_name), value)
  end  

  protected

  def key(gauge_name, at=event_time)
    gauge = @opts[:gauges].fetch(gauge_name)
    gauge.key_at(event_time)
  end

  def event_time
    @event[:time]
  end

end