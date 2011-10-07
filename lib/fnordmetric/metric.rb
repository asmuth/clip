class FnordMetric::Metric
  
  def self.from_options(metric_options)
    return FnordMetric::AverageMetric.new(metric_options) if metric_options[:average]
    return FnordMetric::SumMetric.new(metric_options) if metric_options[:sum]
    return FnordMetric::CountMetric.new(metric_options) if metric_options[:count]
    return FnordMetric::CombineMetric.new(metric_options) if metric_options[:combine]
    raise "please provide one of these options: average, sum, count, combine"
  end

  def initialize(options)
    @options = options
  end

  def current
  	self.at(Time.now)
  end

  def at(time_or_range)
    self.value_at(time_or_range)
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

  def value_at(time_or_range)
    raise "implemented in subclass"
  end

end