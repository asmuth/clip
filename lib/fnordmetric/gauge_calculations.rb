module FnordMetric::GaugeCalculations

  @@avg_per_session_proc = proc{ |_v, _t|
    (_v.to_f / (sync_redis.get(tick_key(_t, :"sessions-count"))||1).to_i)
  }

  @@count_per_session_proc = proc{ |_v, _t|
    (sync_redis.get(tick_key(_t, :"sessions-count"))||0).to_i
  }

  @@avg_per_count_proc = proc{ |_v, _t|
    (_v.to_f / (sync_redis.get(tick_key(_t, :"value-count"))||1).to_i)
  }

  def ticks_in(r, _tick=tick, overflow=0)
    (((r.last-r.first)/_tick.to_f).ceil+1+overflow).times.map{ |n| tick_at(r.first + _tick*(n-1), _tick) }
  end

  def values_in(range)
    ticks = ticks_in(range)
    ticks << tick_at(range.last) if ticks.size == 0
    values_at(ticks)
  end
  
  def value_at(time, opts={}, &block)
    _t = tick_at(time)

    _v = if respond_to?(:_value_at)
      _value_at(key, _t)
    else
      sync_redis.hget(key, _t)
    end

    calculate_value(_v, _t, opts, block)
  end

  def values_at(times, opts={}, &block)
    times = times.map{ |_t| tick_at(_t) }
    Hash.new.tap do |ret|
      if respond_to?(:_values_at)
        _values_at(times, opts={}, &block)
      else
        sync_redis.hmget(key, *times)
      end.each_with_index do |_v, _n|
        _t = times[_n]
        ret[_t] = calculate_value(_v, _t, opts, block)
      end
    end
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
    field_values = sync_redis.zrevrange(
      tick_key(time, opts[:append]),
      0, opts[:max_fields]-1,
      :withscores => true
    )

    unless field_values.first.is_a?(Array)
      field_values = field_values.in_groups_of(2).map do |key, val|
        [key, Float(val)]
      end
    end

    field_values.map do |key, val|
      [key, calculate_value("%.f" % val, time, opts, block)]
    end
  end

  def field_values_total(time)
    (sync_redis.get(tick_key(time, :count))||0).to_i
  end

  def fraction_values_in(range, _append=nil)
    Hash.new{ |h,k| h[k] = [0,0] }.tap do |vals|
      ticks_in(range, retention).each do |_tick|
        sync_redis.hgetall(retention_key(_tick, _append)).each do |k, v|
          kx = k.split("-")
          vals[kx.first.to_i][kx.last == "denominator" ? 1 : 0] += v.to_f
        end
      end
    end
  end

end
