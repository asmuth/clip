module FnordMetric::GaugeModifiers

  def incr(gauge_name, value=1)
    gauge = fetch_gauge(gauge_name)
    assure_two_dimensional!(gauge)
    if gauge.unique? 
      incr_uniq(gauge, value)
    elsif gauge.average? 
      incr_avg(gauge, value)
    else
      incr_tick(gauge, value)
    end
  end

  def incr_tick(gauge, value)
    if gauge.progressive?      
      head = @redis.incrby(gauge.key(:head), value)
      _new = @redis.hsetnx(gauge.key, gauge.tick_at(time), head)
      @redis.hincrby(gauge.key, gauge.tick_at(time), value) unless _new
    else
      @redis.hsetnx(gauge.key, gauge.tick_at(time), 0)
      @redis.hincrby(gauge.key, gauge.tick_at(time), value)
    end
  end

  def incr_uniq(gauge, value, field_name=nil)
    return false if session_key.blank?
    _new = @redis.sadd(gauge.tick_key(time, :sessions), session_key)
    @redis.expire(gauge.tick_key(time, :sessions), gauge.tick)
    if (_new == 1) || (_new == true) #redis vs. em-redis
      sc = @redis.incr(gauge.tick_key(time, :"sessions-count"))
      field_name ? incr_field_by(gauge, field_name, value) : incr_tick(gauge, value)
    end
  end

  def incr_avg(gauge, value)
    @redis.incr(gauge.tick_key(time, :"value-count"))
    incr_tick(gauge, value)
  end

  def incr_field(gauge_name, field_name, value=1)
    gauge = fetch_gauge(gauge_name)
    assure_three_dimensional!(gauge)
    if gauge.unique? 
      incr_uniq(gauge, value, field_name)
    else
      incr_field_by(gauge, field_name, value)
    end
  end

  def incr_field_by(gauge, field_name, value)
    @redis.zincrby(gauge.tick_key(time), value, field_name)
    @redis.incrby(gauge.tick_key(time, :count), 1)
  end

  def set_value(gauge_name, value)
    gauge = fetch_gauge(gauge_name)
    assure_two_dimensional!(gauge)
    @redis.hset(gauge.key, gauge.tick_at(time), value)
  end

  def set_field(gauge_name, field_name, value)
    gauge = fetch_gauge(gauge_name)
    assure_three_dimensional!(gauge)
    @redis.zadd(gauge.tick_key(time), value, field_name)
  end
end

