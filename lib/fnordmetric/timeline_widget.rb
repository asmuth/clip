class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data
    super.merge(
      :start_timestamp => range_to_i.first,
      :end_timestamp => range_to_i.last,
      :tick => tick.to_i,
      :delta => !!@options[:delta],
      :metrics => metrics.map{ |m| m.token },
      :widget_url => "/widget_timeline.js?#{(rand*999).to_i}",
      :chart_type => (@options[:chart] || "line"),
      :widget_height => 320
    )
  end

end