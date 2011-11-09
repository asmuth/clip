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
    assure_two_dimensional!(gauge)
    if gauge.progressive?
      head = @redis.incrby(gauge.key(:head), value).inspect
      @redis.hsetnx(gauge.key, gauge.tick_at(time), head)
    end
    @redis.hincrby(gauge.key, gauge.tick_at(time), value)    
  end  

  def incr_field(gauge_name, field_name, value=1)
    gauge = fetch_gauge(gauge_name)
    assure_three_dimensional!(gauge)
    #here be dragons
  end

  def key(gauge)
    fetch_gauge(gauge).key
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

  def assure_two_dimensional!(gauge)
    return true if gauge.two_dimensional?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with 2-dimensional gauges" 
  end

  def assure_three_dimensional!(gauge)
    return true unless gauge.two_dimensional?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with 3-dimensional gauges" 
  end

end
    