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

  def value_at(time_or_range)
    raise "implemented in subclass"
  end

end