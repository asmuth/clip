class FnordMetric::Context

  include FnordMetric::GaugeModifiers

  class Proxy

    def initialize(_ref)
      @ref = _ref
    end

    def method_missing(method, *args, &block)
      @ref.dispatch(method, *args, &block)
    end

  end

  def initialize(opts, block)
    @block = block
    @opts = opts    
  end

  def call(event, redis)
    @redis = redis
    @event = event    
    proxy.instance_eval(&@block)
  rescue Exception => e
    raise e  if ENV['FNORDMETRIC_ENV'] == 'test'
    puts "error: #{e.message}"
  end

  def proxy
    @proxy ||= Proxy.new(self)
  end

  def dispatch(method, *args, &block)
    if args.size > 0 && multi_gauge?(args.first)
      @opts[:gauges][args.delete_at(0)].send(:"cmd_#{method}", args, &block)
    else
      send(method, *args, &block)
    end
  end

private

  def session_key
    @event[:_session_key]
  end

  def data
  	@event
  end

  def key(gauge)
    fetch_gauge(gauge).key
  end

  def time
    @event[:_time].to_i
  end

protected
  
  def multi_gauge?(_gauge)
    @opts[:gauges].has_key?(_gauge) && @opts[:gauges][_gauge].is_a?(FnordMetric::MultiGauge)
  end

  def fetch_gauge(_gauge)
    _gauge.is_a?(FnordMetric::Gauge) ? _gauge : @opts[:gauges].fetch(_gauge)
  rescue
    error! "error: gauge '#{_gauge}' is undefined"
  end

  def error!(msg)
    FnordMetric.error!(msg)
  end

  def assure_two_dimensional!(gauge)
    return true if gauge.two_dimensional?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with 2-dimensional gauges" 
  end

  def assure_three_dimensional!(gauge)
    return true unless gauge.two_dimensional?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with 3-dimensional gauges" 
  end

  def assure_non_progressive!(gauge)
    return true unless gauge.progressive?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with non-progressive gauges" 
  end

end
    
