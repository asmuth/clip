class FnordMetric::HtmlWidget < FnordMetric::Widget
  def data
    super.merge(
      :html => @opts[:html]
    )
  end

  def add_gauges(gauges)
    @gauges = []
    @tick   = 0
    
    if !gauges.blank?
      error! "initializing a html widget with gauges is void"
    end
  end

  def data_gauges
    {}
  end

  def default_range(now=Time.now)
    0..0
  end

  def has_tick?
    false
  end
end