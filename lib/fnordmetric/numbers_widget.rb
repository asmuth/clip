class FnordMetric::NumbersWidget < FnordMetric::Widget

  def data
    super.merge(
      :offsets => offsets,
      :gauges => data_gauges
    )
  end

  def data_gauges
    Hash.new.tap do |hash|
      gauges.each do |g|
        hash[g.name] = {
          :values => data_gauge(g),
          :title => g.name
        }
      end
    end
  end

  def data_gauge(gauge)
    offsets.map do |offset|
      offset_time = Time.now.to_i - offset*gauge.tick
      [gauge.tick_at(offset_time), gauge.value_at(offset_time)]
    end
  end

  def has_tick?
    false
  end

  def offsets
    [0, 1, 30]
  end

end