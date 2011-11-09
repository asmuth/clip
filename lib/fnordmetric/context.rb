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
    gauge = fetch_gauge(gauge_name)
    error! "error: 'incr' can only be used with numeric gauges" unless gauge.numeric?
    if gauge.progressive?
      head = @redis.incrby(gauge.key_head, value).inspect
      @redis.setnx(gauge.key_at(time), head)
    end
    @redis.incrby(gauge.key_at(time), value)    
  end  

  def key(gauge, at=time)
    fetch_gauge(gauge).key_at(time)
  end

  def time
    @event[:time]
  end

protected

  def fetch_gauge(_gauge)
    _gauge.is_a?(FnordMetric::Gauge) ? _gauge : @opts[:gauges].fetch(_gauge)
  rescue
    error! "error: gauge '#{name}' is undefined"
  end

  def error!(msg)
    puts(msg); exit!
  end

end