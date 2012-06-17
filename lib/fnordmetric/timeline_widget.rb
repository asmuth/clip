class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data
    @series_colors = ["#FACE4F", "#42436B", "#CD645A", "#2F635E"]

    super.merge(
      :gauges => gauges.map(&:name),
      :gauge_titles => gauge_titles,
      :start_timestamp => ticks.first,
      :end_timestamp => ticks.max,
      :autoupdate => (@opts[:autoupdate] || 60),
      :include_current => !!@opts[:include_current],
      :plot_style => (@opts[:plot_style] || 'line'),
      :tick => tick
    )
  end

  def gauge_titles
    {}.tap do |_hash|
      gauges.each do |gauge|
        _hash.merge!(gauge.name => gauge.title)
      end
    end
  end

  def has_tick?
    true
  end

end