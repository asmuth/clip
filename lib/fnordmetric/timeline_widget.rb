class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data_labels
    ticks.map do |t|
      Time.at(t).strftime('%d.%m.%y %H:%M')
    end
  end

  def data_series
    gauges.map do |gauge|
      { 
        :color => '#00f',
        :data => ticks.map{ |t| gauge.value_at(t)||0 }
      }
    end
  end

  def data
    super.merge(
      :labels => data_labels,
      :series => data_series
    )
  end

end