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
  
  def key(_append=nil)
    [@opts[:key_prefix], "gauge", @opts[:key], _append].compact.join("-")
  end

  def two_dimensional?
    !@opts[:three_dimensional]
  end

  def progressive?
    !!@opts[:progressive]
  end

end