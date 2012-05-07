class FnordMetric::ToplistWidget < FnordMetric::Widget

  def data
    super.merge(
      :gauges => data_gauges,
      :autoupdate => (@opts[:autoupdate] || 0),
      :render_target => @opts[:render_target],
      :ticks => @opts[:ticks],
      :tick => tick
    ).tap do |dat|
      dat.merge!(
        :gauges => @opts[:_gauges]
      ) if dat[:ticks]
    end
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