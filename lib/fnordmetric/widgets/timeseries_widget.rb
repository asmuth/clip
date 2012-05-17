class FnordMetric::TimeseriesWidget < FnordMetric::Widget

  def self.execute(namespace, event)
    resp = if event["cmd"] == "values_at"
      {
        :cmd => :values_at,
        :gauges => event["gauges"].map{ |gkey|
          vals = namespace.gauges[gkey.to_sym].values_in(event["since"]..event["until"])
          { :key => gkey, :vals => vals }
        }
      }
    end

    return false unless resp

    resp.merge(
      :class => "widget_response",
      :widget_key => event["widget_key"]
    )
  end

  def data    
    super.merge(
      :series => series,
      :gauges => gauges.map(&:name),
      :start_timestamp => ticks.first,
      :end_timestamp => ticks.last,
      :autoupdate => (@opts[:autoupdate] || 60),
      :include_current => !!@opts[:include_current],
      :default_style => (@opts[:plot_style] || 'line'),
      :async_chart => true,
      :tick => tick
    )
  end

  def series
    colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]

    gauges.map do |gauge|
      { 
        :name => gauge.name, 
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
