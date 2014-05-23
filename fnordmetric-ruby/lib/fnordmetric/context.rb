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

  def call(event, redis, namespace)
    @redis = redis
    @event = event
    @namespace = namespace
    proxy.instance_eval(&@block)
  rescue Exception => e
    raise e if ENV['FNORDMETRIC_ENV'] == 'test'
    puts "error: #{e.message}"
    puts e.backtrace.push("").join("\n") if ENV['FNORDMETRIC_ENV'] == 'dev'
  end

  def proxy
    @proxy ||= Proxy.new(self)
  end

  def namespace
    @namespace
  end

  def dispatch(method, *args, &block)
    if args.size > 0 && gauges[args[0]].try(:renderable?)
      gauges[args.delete_at(0)].execute(method, *args.unshift(self), &block)
    else
      send(method, *args, &block)
    end
  rescue Exception => e
    raise e if ENV['FNORDMETRIC_ENV'] == 'test'
    puts "error: #{e.message}"
    puts e.backtrace.push("\n").join("\n") if ENV['FNORDMETRIC_ENV'] == 'dev'
  end

  def redis_exec(*args)
    @redis.send(*args)
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

  def type
    @event[:_type].to_sym
  end

  def gauges
    @namespace.gauges
  end

protected

  def fetch_gauge(_gauge)
    _gauge.is_a?(FnordMetric::Gauge) ? _gauge : gauges.fetch(_gauge)
  rescue
    error! "error: gauge '#{_gauge}' is undefined"
  end

  def error!(msg)
    FnordMetric.error(msg)
  end


end
