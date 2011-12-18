class FnordMetric::NumbersWidget < FnordMetric::Widget

  def data
    super.merge(
      :gauges => data_gauges
    )
  end

  def data_gauges
    Hash.new.tap do |hash|
      gauges.each do |g|
        hash[g.name] = {
          :tick => g.tick,
          :title => g.title
        }
      end
    end
  end

  def has_tick?
    false
  end

end