class FnordMetric::BarsWidget  < FnordMetric::Widget

  def data
    super.merge(
      :gauges => gauges.map(&:name),
      :gauge_titles => gauge_titles,
      :autoupdate => (@opts[:autoupdate] || 60),
      :order_by => (@opts[:order_by] || 'value'),
      :plot_style => (@opts[:plot_style] || 'vertical'),
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
    false
  end

end