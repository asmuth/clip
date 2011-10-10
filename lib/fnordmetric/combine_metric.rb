class FnordMetric::CombineMetric < FnordMetric::Metric

  def value_at(time_or_range)
    @options[:combine].call(time_or_range)
  end

end