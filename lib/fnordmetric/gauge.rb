class FnordMetric::Gauge
  
  def initialize(opts)
    @opts = opts
  end

  def tick
    (@opts[:tick] || 3600).to_i
  end

  def tick_at(time)    
    (time/tick.to_f).floor*tick
  end
  
  def key_at(time)
    time_part = tick_at(time).to_s
    [key_prefix, @opts[:key], time_part].join("-")
  end

  def key_prefix
    [@opts[:key_prefix], "gauge"].join("-")
  end

  def key_head
    [key_prefix, @opts[:key], "head"].join("-")
  end

  def numeric?
    true
  end

  def progressive?
    !!@opts[:progressive]
  end

end