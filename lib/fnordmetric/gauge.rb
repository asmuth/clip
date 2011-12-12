class FnordMetric::Gauge
  
  include FnordMetric::GaugeCalculations

  def initialize(opts)
    opts.fetch(:key) && opts.fetch(:key_prefix)
    @opts = opts
  end

  def tick
    (@opts[:tick] || 3600).to_i
  end

  def tick_at(time)    
    (time/tick.to_f).floor*tick
  end

  def name
    @opts[:key]
  end
  
  def key(_append=nil)
    [@opts[:key_prefix], "gauge", name, tick, _append].flatten.compact.join("-")
  end

  def tick_key(_time, _append=nil)
    key([(progressive? ? :progressive : tick_at(_time).to_s), _append])
  end

  def two_dimensional?
    !@opts[:three_dimensional]
  end

  def progressive?
    !!@opts[:progressive]
  end

  def unique?
    !!@opts[:unique]
  end

  def average?
    !!@opts[:average]
  end

  def add_redis(_redis)
    @opts[:redis] = _redis
  end

end