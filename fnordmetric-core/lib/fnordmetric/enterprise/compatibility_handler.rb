module FnordMetric::Enterprise

  CompatibilityHandler = proc do
    if data[:_cmd]
     cmd = data[:_cmd].to_s
    else
      FnordMetric.error("missing key: _cmd")
      next
    end

    if m = cmd.match(/^SAMPLE (.*)(delta|mean|sum)-([0-9]+) ([0-9]+)$/)
      gauge_type = m[2].to_sym
      gauge_tick = m[3].to_i
      gauge_key = :"#{m[1]}#{gauge_type}-#{gauge_tick}"
      op_value = m[4].to_i
    else
      FnordMetric.error("invalid _cmd")
      next
    end

    gauge = if namespace.gauges.has_key?(gauge_key)
      namespace.gauges[gauge_key]
    else
      namespace.opt_gauge(gauge_key,
        :flush_interval => gauge_tick,
        :average => (gauge_type == :mean),
        :zero_config => true)
    end

    case gauge_type

      when :delta, :sum
        incr gauge, op_value

      when :mean
        incr_avg gauge, op_value

    end

  end

end
