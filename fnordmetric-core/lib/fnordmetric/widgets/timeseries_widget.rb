class FnordMetric::TimeseriesWidget < FnordMetric::Widget

  def self.execute(namespace, event)
    resp = if event["cmd"] == "values_at"
      {
        :cmd => :values_at,
        :gauges => event["gauges"].map{ |gkey|
          _gauge = namespace.gauges[gkey.to_sym]
          vals = _gauge.values_in(event["since"]..event["until"])
          { :key => gkey, :vals => vals, :title => _gauge.title }
        }
      }
    end

    return false unless resp

    resp.merge(
      :type => "widget_response",
      :widget_key => event["widget_key"]
    )
  end

  def data
    super.merge(
      :series => series,
      :gauges => gauges.map(&:name),
      :start_timestamp => ticks.first,
      :end_timestamp => ticks.last,
      :xticks => (@opts[:xticks] || 30),
      :autoupdate => (@opts[:autoupdate] || 60),
      :include_current => !!@opts[:include_current],
      :default_style => (@opts[:plot_style] || 'line'),
      :async_chart => true,
      :tick => tick
    )
  end

  def series
    colors = FnordMetric::COLORS.dup

    gauges.map do |gauge|
      {
        :name => gauge.name,
        :title => gauge.title,
        :data => [{:x => ticks.first, :y => 0}],
        :color => colors.unshift(colors.pop).first
      }
    end
  end

  def has_tick?
    true
  end

end

class FnordMetric::TimelineWidget < FnordMetric::TimeseriesWidget
end
