module FnordMetric::GaugeCalculations

  @@avg_per_session_proc = proc{ |_v, _t|
    #raise redis.get(tick_key(_t, :"sessions-count")).inspect
    (_v.to_f / (redis.get(tick_key(_t, :"sessions-count"))||0).to_i)
  }

  def value_at(time, opts={}, &block)
    _t = tick_at(time)
    _v = redis.hget(key, _t)

    calculate_value(_v, _t, opts, block)
  end

  def values_at(times, opts={}, &block)
    times = times.map{ |_t| tick_at(_t) }
    Hash.new.tap do |ret|
      redis.hmget(key, *times).each_with_index do |_v, _n|
        _t = times[_n]
        ret[_t] = calculate_value(_v, _t, opts, block)
      end
    end
  end

  def values_in(range, opts={}, &block)
    values_at((tick_at(range.first)..range.last).step(tick))
  end

  def calculate_value(_v, _t, opts, block)
    block = @@avg_per_session_proc if opts[:avg_per_session]

    if block
      instance_exec(_v, _t, &block)
    else
      _v
    end
  end

  def redis
    @opts[:redis]
  end

end