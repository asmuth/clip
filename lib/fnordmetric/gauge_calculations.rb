module FnordMetric::GaugeCalculations

  def value_at(time, opts={}, &block)
    _t = tick_at(time)
    _v = redis.hget(key, _t)

    if block
      instance_exec(_v, _t, &block)
    elsif opts[:avg_per_session]
      value_per_session(_v, _t)
    else
      _v
    end
  end

  def value_per_session(_v, _t)
    (_v.to_f / (redis.get(tick_key(_t, :"sessions-count"))||0).to_i)
  end

  def redis
    @opts[:redis]
  end

end