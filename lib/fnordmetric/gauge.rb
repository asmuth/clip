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
    [@opts[:gauge_key], @opts[:key], time_part].join("-")
  end

end