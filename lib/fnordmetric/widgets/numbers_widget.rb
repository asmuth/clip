class FnordMetric::NumbersWidget < FnordMetric::Widget

  def self.execute(namespace, event)
    puts "XEAFFFFH -> #{namespace.gauges.has_key?(event["gauge"].to_sym)} - #{event}"

    resp = if event["cmd"] == "values_for"
      execute_values_for(namespace.gauges[event["gauge"].to_sym], event) 
    end

    return false unless resp

    resp.merge(
      :class => "widget_response",
      :widget_key => event["widget_key"]
    )
  end

  def self.execute_values_for(gauge, event)
    _t = Time.now.to_i

    puts "FUUUU #{gauge.inspect}"

    values = {}.tap do |out|
      out["#{gauge.name}-now"]  = { 
        :value => gauge.value_at(_t),
        :desc  => "$formatTimeRangePre(#{gauge.tick}, 0)"
      }
      out["#{gauge.name}-last"]  = { 
        :value => gauge.value_at(_t-gauge.tick),
        :desc  => "$formatTimeRangePre(#{gauge.tick}, -1)"
      }
    end

    { "cmd" => "values_for", 
      "series" => gauge.name, 
      "values" => values }
  end

  def data
    super.merge(
      :series => gauges.map(&:name),
      :offsets => (@opts[:offsets] || [0, 1, "s30"]),
      :autoupdate => (@opts[:autoupdate] || 0)
    )
  end

  def has_tick?
    false
  end
  
end