class FnordMetric::Reactor

  def initialize
    @redis = FnordMetric.mk_redis
    @namespaces = FnordMetric.namespaces
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
    return false unless event["namespace"]
    return false unless ns = @namespaces[event["namespace"].to_sym]
    messages << discover(ns) if event["type"] == "discover_request"
    messages << widget(ns, event) if event["type"] == "widget_request"
    messages << gauge(ns, event) if event["type"] == "render_request"
    messages << active_users(ns, event) if event["type"] == "active_users_request"
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

end
