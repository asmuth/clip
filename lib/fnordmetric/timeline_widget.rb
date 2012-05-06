class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data
    @series_colors = ["#FACE4F", "#42436B", "#CD645A", "#2F635E"]

    super.merge(
      :autoupdate => (@opts[:autoupdate] || 60),
      :include_current => !!@opts[:include_current],
      :plot_style => (@opts[:plot_style] || 'line'),
      :render_target => @opts[:render_target],
      :height => @opts[:height]
    ).tap do |dat|
      dat.merge!(
        :gauges => gauges.map(&:name),
        :gauge_titles => gauge_titles,
        :start_timestamp => ticks.first,
        :end_timestamp => ticks.last,
        :tick => tick
      ) if has_tick?
      dat.merge!(
        :gauges => @opts[:_gauges],
        :gauge_titles => @opts[:_gauge_titles],
        :tick => @opts[:ticks].first,
        :ticks => @opts[:ticks]
      ) unless has_tick?
    end
  end

  def gauge_titles
    {}.tap do |_hash|
      gauges.each do |gauge|
        _hash.merge!(gauge.name => gauge.title)
      end
    end
  end

  def has_tick?
    @opts[:multi_tick] ? false : true
  end

end