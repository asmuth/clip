class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data
    super.merge(
      :labels => ["fnord", "snafu", "blubb", "foo", "bar", "boing"],
      :series => [
        { color: '#00f', data: [5,95,21,74,12,64] } 
      ]
    )
  end

end