class FnordMetric::TimeseriesWidget < FnordMetric::Widget

  def data    
    super.merge(
      :series => series,
      :start_timestamp => ticks.first,
      :end_timestamp => ticks.last,
      :autoupdate => (@opts[:autoupdate] || 60),
      :include_current => !!@opts[:include_current],
      :plot_style => plot_style,
      :tick => tick
    )
  end

  def plot_style
    (@opts[:plot_style] || 'line').to_s.gsub("area", "stack")
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
