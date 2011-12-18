class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data
    @series_colors = ["#FACE4F", "#42436B", "#CD645A", "#2F635E"]

    super.merge(
      :gauges => gauges.map(&:name),
      :start_timestamp => ticks.first,
      :end_timestamp => ticks.last,
      :tick => tick
    )
  end

  def has_tick?
    true
  end

end