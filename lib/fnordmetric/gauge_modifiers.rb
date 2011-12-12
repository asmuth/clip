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
        @redis.hsetnx(gauge.key, gauge.tick_at(time), head).callback do |_new|
          @redis.hincrby(gauge.key, gauge.tick_at(time), value) unless _new
        end
      end
    else
      @redis.hsetnx(gauge.key, gauge.tick_at(time), 0).callback do
        @redis.hincrby(gauge.key, gauge.tick_at(time), value)
      end
    end
  end  

  def incr_uniq(gauge, value)
    return false if session_key.blank?
    @redis.sadd(gauge.tick_key(time, :sessions), session_key).callback do |_new|
      @redis.expire(gauge.tick_key(time, :sessions), gauge.tick)
      if _new == 1
        @redis.incr(gauge.tick_key(time, :"sessions-count")).callback do |sc|
          incr_tick(gauge, value)
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
    # here be dragons
  end

end