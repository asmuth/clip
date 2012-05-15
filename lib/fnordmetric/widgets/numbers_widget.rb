class FnordMetric::NumbersWidget < FnordMetric::Widget

  def data
    super.merge(
      :series => gauges.map(&:name),
      :offsets => (@opts[:offsets] || [0, 1, "s30"]),
      :autoupdate => (@opts[:autoupdate] || 0)
    )
  end

  def has_tick?
    false
  end
  
end