class FnordMetric::BarsWidget  < FnordMetric::Widget

  def self.execute(namespace, event)
    resp = if event["cmd"] == "values_for"
      {
        :cmd => :values_for,
        :values => execute_values_for(namespace.gauges[event["gauge"].to_sym], event["until"])
      }
    end

    return false unless resp

    resp.merge(
      :type => "widget_response",
      :widget_key => event["widget_key"]
    )
  end

  def self.execute_values_for(gauge, time)
    gauge.field_values_at(time).sort do |a,b|
      a.first.to_i <=> b.first.to_i
    end.map do |a|
      [a.first, a.second.to_i]
    end
  end

  def data
    super.merge(
      :gauge => gauges.first.name,
      :title => gauges.first.title,
      :autoupdate => (@opts[:autoupdate] || 60),
      :order_by => (@opts[:order_by] || 'value'),
      :plot_style => (@opts[:plot_style] || 'vertical'),
      :async_chart => true,
      :color => FnordMetric::COLORS.last,
      :tick => tick
    )
  end

  def has_tick?
    false
  end

end
