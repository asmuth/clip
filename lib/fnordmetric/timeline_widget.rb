class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data
    super.merge(
      :blubb => "bar"
    )
  end

end