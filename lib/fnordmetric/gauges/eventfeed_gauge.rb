class FnordMetric::EventfeedGauge < FnordMetric::Gauge

  def render(namespace, event)
    render_page(:eventfeed_gauge)
  end

  def execute(cmd, context, *args)
    FnordMetric.error("gauge '#{name}': unknown command: #{cmd}")
  end

  def renderable?
    true
  end

  def has_tick?
    false
  end

end