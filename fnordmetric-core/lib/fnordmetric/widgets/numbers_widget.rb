class FnordMetric::NumbersWidget < FnordMetric::Widget

  def self.execute(namespace, event)
    resp = if event["cmd"] == "values_for"
      execute_values_for(namespace.gauges[event["gauge"].to_sym], event) 
    end

    return false unless resp

    resp.merge(
      :type => "widget_response",
      :widget_key => event["widget_key"]
    )
  end

  def self.execute_values_for(gauge, event)
    unless gauge
      return { "error" => "gauge not found..." }
    end

    _t = Time.now.to_i

    if at = event["at"]
      value = if at =~ /sum\((.+)\)/
        vals = gauge.values_in(FnordMetric::Util.parse_time($1).._t+gauge.tick)
        vals.values.compact.map(&:to_f).inject(&:+)
      elsif at =~ /avg\((.+)\)/
        vals = gauge.values_in(FnordMetric::Util.parse_time($1).._t+gauge.tick)
        (vals.values.compact.map(&:to_f).inject(&:+) || 0) / vals.size.to_f
      else
        gauge.value_at(FnordMetric::Util.parse_time(at)).to_i
      end

      return({
        "cmd" => "values_for",
        "at" => event["at"],
        "gauge" => gauge.name,
        "value" => value })
    end

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
      "gauge" => gauge.name,
      "values" => values }
  end

  def data
    super.merge(
      :series => gauges.map(&:name),
      :series_titles => gauges.map(&:title),
      :series_units => Hash[gauges.map{ |g| [g.name, g.unit] }],
      :offsets => (@opts[:offsets] || [0, 1, "s30"]),
      :autoupdate => (@opts[:autoupdate] || 60)
    )
  end

  def has_tick?
    false
  end
  
end
