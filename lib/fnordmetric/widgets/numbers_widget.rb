class FnordMetric::NumbersWidget < FnordMetric::Widget

  def self.execute(namespace, event)
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

    values = {}.tap do |out|
      event["offsets"].each do |off|
        if off.to_s.starts_with?("s")
          offset = 0
          span = (gauge.tick * off.to_s[1..-1].to_i)
          values = gauge.values_in((_t-span).._t+gauge.tick)
          value = values.values.compact.map(&:to_i).sum
        else
          offset = off.to_i * gauge.tick
          span = gauge.tick
          value = gauge.value_at(_t-offset)
        end

        out["#{gauge.name}-#{offset}-#{span}"]  = { 
          :value => value,
          :desc  => "$formatOffset(#{offset}, #{span})"
        }
      end
    end

    { "cmd" => "values_for", 
      "series" => gauge.name, 
      "values" => values }
  end

  def data
    super.merge(
      :series => gauges.map(&:name),
      :offsets => (@opts[:offsets] || [0, 1, "s30"]),
      :autoupdate => (@opts[:autoupdate] || 60)
    )
  end

  def has_tick?
    false
  end
  
end