class FnordMetric::Context

  include FnordMetric::GaugeModifiers

  def initialize(opts, block)
    @block = block
  	@opts = opts    
  end

  def call(event, redis)
    @redis = redis
  	@event = event    
  	self.instance_eval(&@block)
  rescue Exception => e
   puts "error: #{e.message}"
  end

  private

  def data
  	@event
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
    
