class FnordMetric::Metric

  METRIC_TYPES = %w(count average sum combine median)

  def self.from_options(options)
    if (klass_name = METRIC_TYPES.detect{ |n| !!options[n.intern] })
      klass = "FnordMetric::#{klass_name.classify}Metric".constantize
      return klass.new(options)
    end
    raise "please provide one of these options: average, median, sum, count, combine"
  end

  def initialize(options)
    @options = options
  end

  def current
    self.at(Time.now)
  end

  def at(time_or_range)
    if cache_this?(time_or_range) && (_v=try_cache(time_or_range))
      _v # cache hit
    else # cache miss
      value_at(time_or_range).tap do |_v|
        store_cache(time_or_range, _v) if cache_this?(time_or_range)
      end
    end
  end

  def token
    @options[:name]
  end

  def events
    _events = FnordMetric::Event
    if @options[:types]
      _events = _events.where(:type.in => [@options[:types]].flatten)
    end
    _events
  end

  def events_at(time_or_range)
    if time_or_range.is_a?(Range)
      events.where(:time.lt => time_or_range.last.to_i).where(:time.gt => time_or_range.first.to_i)
    else
      events.where(:time.lt => time_or_range.to_i)
    end
  end

private

  def value_at(time_or_range)
    raise "implemented in subclass"
  end

  def cache_this?(time_or_range)
    ((!time_or_range.is_a?(Range) && time_or_range.to_i < Time.now.to_i) ||
    (time_or_range.is_a?(Range) && time_or_range.last.to_i < Time.now.to_i))
  end

  def try_cache(time_or_range)
    FnordMetric::Cache.get(cache_key(time_or_range))
  end

  def store_cache(time_or_range, value)
    FnordMetric::Cache.store!(cache_key(time_or_range), value)
  end

  def cache_key(time_or_range)
    time_part = if time_or_range.is_a?(Range)
      "r#{time_or_range.first.to_i}-#{time_or_range.last.to_i}"
    else
      "t#{time_or_range.to_i.to_s}"
    end
    [self.token, time_part].join("|")
  end


end
