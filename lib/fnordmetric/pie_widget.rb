class FnordMetric::PieWidget  < FnordMetric::Widget

  def data
    super.merge(
      :gauges => gauges.map(&:name),
      :gauge_titles => gauge_titles,
      :autoupdate => (@opts[:autoupdate] || 60)
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
    false
  end

end