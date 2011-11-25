class FnordMetric::TimelineWidget  < FnordMetric::Widget

  def data_labels
    ticks.map do |t|
      Time.at(t).strftime('%d.%m.%y %H:%M')
    end
  end

  def data_series
    gauges.map do |gauge|
      { 
        :color => next_series_colour,
        :data => ticks.map{ |t| gauge.value_at(t)||0 }
      }
    end
  end

  def next_series_colour
    @series_colors.pop.tap do |color|
      @series_colors.unshift(color)
    end
  end

  def data
    @series_colors = ["#FACE4F", "#42436B", "#CD645A", "#2F635E"]

    super.merge(
      :labels => data_labels,
      :series => data_series
    )
  end

end