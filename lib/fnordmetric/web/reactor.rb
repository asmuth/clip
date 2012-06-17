class FnordMetric::Reactor

  def initialize
    @namespaces = FnordMetric.namespaces
  end

  def execute(*args)
    execute_unsafe(*args)
  rescue Exception => e
    FnordMetric.error("reactor crashed: " + e.to_s); []
  end

private

  def execute_unsafe(socket, event, messages = [])
    return false unless event["namespace"]
    return false unless ns = @namespaces[event["namespace"].to_sym]
    messages << discover(ns) if event["type"] == "discover_request"
    messages << widget(ns, event) if event["type"] == "widget_request"
    messages << gauge(ns, event) if event["type"] == "render_request"
    messages.flatten.compact
  end

  def widget(namespace, event)
    "FnordMetric::#{event["klass"]}".constantize.execute(namespace, event) # FIXPAUL
  end

  def gauge(namespace, event)
    return false unless gauge = namespace.gauges[event["gauge"].to_sym]

    {
      :type    => "render_response",
      :gauge   => gauge.name,
      :payload => gauge.render_to_event(namespace, event)
    }
  end

  def discover(namespace)
    [namespace.dashboards.map do |dash_key, dash|
      { "type" => "discover_response", "gauge_key" => dash_key, "view" => "dashboard",
        "group" => dash.group }
    end,
    namespace.gauges.map do |gauge_key, gauge|
      next unless gauge.renderable?
      { "type" => "discover_response", "gauge_key" => gauge_key, "view" => "gauge",
        "title" => gauge.title, "group" => gauge.group, "tick" => gauge.tick }
    end.compact]
  end

end