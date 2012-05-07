module FnordMetric::GaugeCalculations

  @@avg_per_session_proc = proc{ |_v, _t|
    (_v.to_f / (redis.get(tick_key(_t, :"sessions-count"))||1).to_i)
  }

  @@count_per_session_proc = proc{ |_v, _t|
    (redis.get(tick_key(_t, :"sessions-count"))||0).to_i
  }

  @@avg_per_count_proc = proc{ |_v, _t|
    (_v.to_f / (redis.get(tick_key(_t, :"value-count"))||1).to_i)
  }

  def value_at(time, opts={}, &block)
    _t = tick_at(time)

    _v = if respond_to?(:_value_at)
      _value_at(key, _t)
    else
      redis.hget(key, _t)
    end

    calculate_value(_v, _t, opts, block)
  end

  def values_at(times, opts={}, &block)
    times = times.map{ |_t| tick_at(_t) }
    Hash.new.tap do |ret|
      if respond_to?(:_values_at)
        _values_at(times, opts={}, &block)
      else
        redis.hmget(key, *times)
      end.each_with_index do |_v, _n|
        _t = times[_n]
        ret[_t] = calculate_value(_v, _t, opts, block)
      end
    end
  end

  def values_in(range, opts={}, &block)
    values_at((tick_at(range.first)..range.last).step(tick))
  end

  def calculate_value(_v, _t, opts, block)
    block = @@avg_per_count_proc if average?
    #block = @@count_per_session_proc if unique?
    block = @@avg_per_session_proc if unique? && average?
    
    if block
      instance_exec(_v, _t, &block)
    else
      _v
    end
  end

  def field_values_at(time, opts={}, &block)
    opts[:max_fields] ||= 50
    redis.zrevrange(
      tick_key(time), 
      0, opts[:max_fields]-1, 
      :withscores => true
    ).in_groups_of(2).map do |key, val|
      [key, calculate_value(val, time, opts, block)]
    end
  end

  def field_values_total(time)
    (redis.get(tick_key(time, :count))||0).to_i
  end

  def redis
    @opts[:redis]
  end

end