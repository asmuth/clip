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
      @redis.incrby(gauge.key(:head), value).callback do |head|
        _head_tick = gauge.tick_at(time)
        @redis.hsetnx(gauge.key, _head_tick, head).callback do |_new|
          if _new
            @redis.get gauge.key(:head_tick) do |_last_head_tick|
              unless _last_head_tick.blank?
                if _last_head_tick.to_i + gauge.tick.to_i < _head_tick.to_i
                  _ticks_range = ((_last_head_tick.to_i)...(_head_tick.to_i)).step(gauge.tick)
                  @redis.hmset gauge.key, *(_ticks_range[1..-1].map{|_t| [_t, head]}.flatten)
                end
              end
            end
            @redis.set(gauge.key(:head_tick), _head_tick)
          else
            @redis.hincrby(gauge.key, gauge.tick_at(time), value)
          end
        end
      end
    else
      @redis.hsetnx(gauge.key, gauge.tick_at(time), 0).callback do
        @redis.hincrby(gauge.key, gauge.tick_at(time), value)
      end
    end
  end  

  def incr_uniq(gauge, value, field_name=nil)
    return false if session_key.blank?
    @redis.sadd(gauge.tick_key(time, :sessions), session_key).callback do |_new|
      @redis.expire(gauge.tick_key(time, :sessions), gauge.tick)
      if (_new == 1) || (_new == true) #redis vs. em-redis
        @redis.incr(gauge.tick_key(time, :"sessions-count")).callback do |sc|
          field_name ? incr_field_by(gauge, field_name, value) : incr_tick(gauge, value)
        end
      end
    end
  end

  def incr_avg(gauge, value)
    @redis.incr(gauge.tick_key(time, :"value-count")).callback do
      incr_tick(gauge, value)
    end
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
    @redis.zincrby(gauge.tick_key(time), value, field_name).callback do
      @redis.incrby(gauge.tick_key(time, :count), 1)
    end
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