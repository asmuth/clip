class FnordMetric::CountMetric < FnordMetric::Metric

private

  def value_at(time_or_range)
    events_at(time_or_range).count
  end

end