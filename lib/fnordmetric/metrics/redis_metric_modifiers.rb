module FnordMetric::RedisMetricModifiers

  def incr(time, value=1)
    assure_two_dimensional!(self)
    if self.unique? 
      incr_uniq(time, value)
    elsif self.average? 
      incr_avg(time, value)
    else
      incr_tick(time, value)
    end
  end

  def incr_tick(time, value)
    if self.progressive?      
      redis.incrby(self.key(:head), value).callback do |head|
        redis.hsetnx(self.key, self.tick_at(time), head).callback do |_new|
          redis.hincrby(self.key, self.tick_at(time), value) unless _new
        end
      end
    else
      redis.hsetnx(self.key, self.tick_at(time), 0).callback do
        redis.hincrby(self.key, self.tick_at(time), value)
      end
    end
  end  

  def incr_uniq(time, value, field_name=nil)
    return false if session_key.blank?
    redis.sadd(self.tick_key(time, :sessions), session_key).callback do |_new|
      redis.expire(self.tick_key(time, :sessions), self.tick)
      if (_new == 1) || (_new == true) #redis vs. em-redis
        redis.incr(time, self.tick_key(time, :"sessions-count")).callback do |sc|
          field_name ? incr_field_by(time, field_name, value) : incr_tick(time, value)
        end
      end
    end
  end

  def incr_avg(time, value)
    redis.incr(self.tick_key(time, :"value-count")).callback do
      incr_tick(time, value)
    end
  end

  def incr_field(time, field_name, value=1)
    assure_three_dimensional!(self)
    if self.unique? 
      incr_uniq(time, value, field_name)
    else
      incr_field_by(time, field_name, value)
    end
  end

  def incr_field_by(time, field_name, value)
    redis.zincrby(self.tick_key(time), value, field_name).callback do
      redis.incrby(self.tick_key(time, :count), 1)
    end
  end  

  def set_value(time, value)
    assure_two_dimensional!(self)
    redis.hset(self.key, self.tick_at(time), value)
  end

  def set_field(time, field_name, value)
    assure_three_dimensional!(self)
    redis.zadd(self.tick_key(time), value, field_name)
  end

  def assure_two_dimensional!(gauge)
    return true if gauge.two_dimensional?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with 2-dimensional gauges" 
  end

  def assure_three_dimensional!(gauge)
    return true unless gauge.two_dimensional?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with 3-dimensional gauges" 
  end

  def assure_non_progressive!(gauge)
    return true unless gauge.progressive?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with non-progressive gauges" 
  end

end