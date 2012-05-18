class FnordMetric::ToplistGauge < FnordMetric::Gauge

  def render(namespace, event)
    interval = parse_interval(event["interval"])
    colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]


    render_page(:toplist_gauge)
  end

  def execute(cmd, context, *args)
    #incr_series(context, *args) if cmd == :incr
  end

  def renderable?
    true
  end

end