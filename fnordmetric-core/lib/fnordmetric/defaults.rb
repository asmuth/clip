FnordMetric::COLORS = ["#4572a7", "#aa4643", "#89a54e", "#80699b", "#3d96ae", "#db843d"].reverse

FnordMetric::TICKS = lambda{ |tick, span| [tick, 60, 300, 1200, 3600, 86400]
  .select{ |t| (t >= tick) && ((span/t) > 5) }
  .uniq }

FnordMetric::DEFAULT_PROC = lambda{ |arg| }

FnordMetric::ZERO_CONFIG_TYPES = [:_incr, :_decr, :_avg, :_min, :_max, :_set]

FnordMetric::ZERO_CONFIG_HANDLER = proc {
  if data[:gauge]
    gauge_key = data[:gauge].to_sym
  else
    FnordMetric.error("missing key for zero config event: gauge")
    next
  end

  unless data[:flush_interval]
    FnordMetric.error("missing key for zero config event: flush_interval")
    next
  end

  unless data[:value]
    FnordMetric.error("missing key for zero config event: value")
    next
  end

  gauge = if namespace.gauges.has_key?(gauge_key)
    namespace.gauges[gauge_key]
  else
    namespace.opt_gauge(gauge_key,
      :tick => data[:flush_interval].to_i,
      :average => (type == :_avg),
      :zero_config => true)
  end

  case type

    when :_set
      set_value gauge, data[:value]

    when :_incr
      incr_tick gauge, data[:value]

    when :_decr
      FnordMetric.error("_decr is not yet implemented")

    when :_avg
      incr_avg gauge, data[:value]

    when :_min, :_max
      FnordMetric.error("_min/_max is not yet implemented")

  end
}
