class FnordMetric::SumMetric < FnordMetric::Metric

private

  def value_at(time_or_range)
    # FIXME: value_at(my_time) is really slow, because it has to fetch all events that 
    # happened until that time (and this is not even cached, when time is in the future     
    # or just now), so we should redirect the call to something like:
    # value_at(last_cache_since_my_time) + value_at(last_cache_since_my_time..my_time) 
    events_at(time_or_range).sum(:"data.#{@options[:sum]}")
  end

end