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
    _v = redis.hget(key, _t)

    _ret = calculate_value(_v, _t, opts, block)

    if progressive? && _ret.blank?
      _head_tick = redis.get key(:head_tick)
      unless _head_tick.blank?
        _head_tick = _head_tick.to_i
        unless _head_tick.blank?
          _head_value = redis.get key(:head)
          if _head_tick > _t.to_i
            _ret = 0
          else
            _ret = _head_value
          end
        end
      end
    end

    _ret
  end

  def values_at(times, opts={}, &block)
    times = times.map{ |_t| tick_at(_t) }
    Hash.new.tap do |ret|

      if progressive?
        _head_tick = redis.get key(:head_tick)
        unless _head_tick.blank?
          _head_tick  = _head_tick.to_i
          _head_value = redis.get key(:head)
        end
        redis.hmget(key, *times).each_with_index do |_v, _n|
          _t = times[_n]
          ret[_t] = calculate_value(_v, _t, opts, block)

          if !_head_tick.blank? && ret[_t].blank?
            if _head_tick > _t.to_i
              ret[_t] = 0
            else
              ret[_t] = _head_value
            end
          end
        end
      else
        redis.hmget(key, *times).each_with_index do |_v, _n|
          _t = times[_n]
          ret[_t] = calculate_value(_v, _t, opts, block)
        end
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
    field_values = redis.zrevrange(
      tick_key(time), 
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
    (redis.get(tick_key(time, :count))||0).to_i
  end

  def redis
    @opts[:redis]
  end

end