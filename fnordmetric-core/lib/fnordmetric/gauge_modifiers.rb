module FnordMetric::GaugeModifiers

  def incr(gauge_name, value=1)
    value = value.to_i
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
    @redis.hincrby(gauge.key(:"mean-counts"), gauge.tick_at(time), 1).callback do
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

  def incr_numerator(ctx, series_name=:default, value=1, prog=false)
    incr_fraction(ctx, series_name, :numerator, value, prog)
  end

  def incr_denominator(ctx, series_name=:default, value=1, prog=false)
    incr_fraction(ctx, series_name, :denominator, value, prog)
  end

  def incr_fraction(ctx, series_name, part, value, prog)
    return unless series_name = assure_series_exists!(series_name)
    assure_has_series!

    at = ctx.send(:time)
    value = parse_numeric(value)

    if prog
      raise "FIXPAUL: not yet implemented: progressive fraction gauges"
    end

    ctx.redis_exec(:hincrby, retention_key(at, series_name), "#{tick_at(at)}-#{part}", value).callback do 
      ctx.redis_exec :expire,  retention_key(at, series_name)
    end
  end
  
  def assure_has_series!
    return true if has_series?
    error! "error: #{caller[0].split(" ")[-1]} can only be used with series gauges" 
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

  def assure_series_exists!(series_name)
    if series_name == :default && @opts[:series].size > 1
      error! "gauge '#{name}': don't know which series to increment"
    elsif series_name == :default
      return @opts[:series].first
    elsif !series_name.respond_to?(:to_sym) || !@opts[:series].include?(series_name.to_sym)
      error! "gauge '#{name}': unknown series: #{series_name}"
    else 
      return series_name
    end
  end

  def parse_numeric(val)
    if val.is_a?(Numeric)
      return val
    elsif val.is_a?(String) && val.match(/[0-9]+/)
      val.to_i
    elsif val.is_a?(String) && val.match(/[0-9]+(\.|,)[0-9]+/)
      val.to_f
    else
      error! "gauge '#{name}': incr called with non-numerical value: #{val}"
    end
  end

end
