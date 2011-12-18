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
    @redis.hsetnx(gauge.tick_key(time), field_name, 0).callback do
      @redis.hincrby(gauge.tick_key(time), field_name, value).callback do 
        @redis.incrby(gauge.tick_key(time, :count), 1)
      end
    end
  end  
  
end