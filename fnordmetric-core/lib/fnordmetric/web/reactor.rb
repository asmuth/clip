class FnordMetric::Reactor

  def initialize
    @redis = FnordMetric.mk_redis
    @namespaces = FnordMetric.namespaces.dup
  end

  def ready!
    @namespaces.each do |key, ns|
      ns.ready!(@redis)
    end
  end

  def execute(*args)
    execute_unsafe(*args)
  rescue Exception => e
    FnordMetric.error("reactor crashed: " + e.to_s) 
    puts (e.backtrace * "\n") if ENV["FNORDMETRIC_ENV"] == "dev"
    []
  end

private

  def execute_unsafe(socket, event, messages = [])
    return [] unless event["namespace"]

    unless ns = @namespaces[event["namespace"].to_sym]
      return([{ "error" => "invalid namespace: #{event["namespace"]}" }])
    end

    messages << discover(ns) if event["type"] == "discover_request"
    messages << widget(ns, event) if event["type"] == "widget_request"
    messages << gauge(ns, event) if event["type"] == "render_request"
    messages << active_users(ns, event) if event["type"] == "active_users_request"
    messages << gauge_list(ns, event) if event["type"] == "gauge_list_request"

    messages.flatten.compact.map do |m|
      m["namespace"] = event["namespace"]; m
    end
  end

  def widget(namespace, event)
    klass = if event["klass"] == "generic" && event["cmd"] == "values_for"
      FnordMetric::NumbersWidget
    elsif event["klass"] == "generic" && event["cmd"] == "values_at"
      FnordMetric::TimeseriesWidget
    else
      "FnordMetric::#{event["klass"]}".constantize
    end

    klass.execute(namespace, event)
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
    namespace.ready!(@redis)

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

  def active_users(namespace, event)
    namespace.ready!(@redis)

    events = if event["filter_by_type"]
      namespace.events(:by_type, :type => event["type"])
    elsif event["filter_by_session_key"]
      namespace.events(:by_session_key, :session_key => params["session_key"])
    else
      find_opts = { :limit => 100 }
      find_opts.merge!(:since => event["since"].to_i+1) if event["since"]
      namespace.events(:all, find_opts)
    end

    sessions = namespace.sessions(:all, :limit => 100).map do |session|
      session.fetch_data!
      session.to_json
    end

    types_key = namespace.key_prefix("type-")
    types = if event["first_poll"]
      @redis.keys("#{types_key}*").map{ |k| k.gsub(types_key,'') }
    else
      []
    end

    {
      :type => "active_users_response",
      :sessions => sessions,
      :events => events.map(&:to_json),
      :types => types
    }
  end

  def gauge_list(namespace, event)
    namespace.ready!(@redis)

    gauges = namespace.gauges.map do |name, gauge|
      {
        "key" => gauge.name,
        "title" => gauge.title
      }
    end

    {
      :type   => "gauge_list_response",
      :gauges => gauges
    }
  end

end
