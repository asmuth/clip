class FnordMetric::MultiGauge < FnordMetric::RemoteGauge

  def initialize(opts)
    opts.fetch(:key)
    @opts = opts
    @widgets = {}

    FnordMetric.register(self)
  end

  def name
    @opts[:key]
  end

  def title
    @opts[:title] || name
  end

  def key_nouns
    @opts[:key_nouns] || ["Key", "Keys"]
  end
  
  def key(_append=nil)
    ["FIXPAUL-KEYPREFIX", "multigauge", name, _append].flatten.compact.join("-")
  end

  def react(ev)
    render_self!       if ev["_class"] == "render_request"
    render_widget!(ev) if ev["_class"] == "widget_request"
  end

private

  def render_self!
    payload = {
      :title => title,
      :gauge_key => name,
      :widgets => Hash[@widgets.map{|k,w|[k,w.opts]}]
    }

    respond(
      :_class => "render_response", 
      :payload => payload
    )
  end

  def render_widget!(ev)
    puts ev.inspect

    if @widgets.has_key?(ev["widget_key"])
      respond(@widgets[ev["widget_key"]].react(ev).merge(
        :_class => "widget_response"
      ))
    end
  end

  def widget(klass, opts)
    opts[:gauge_key] ||= name

    [
      "FnordMetric",
      "#{klass.to_s.capitalize}Widget"
    ].join("::").constantize.new(opts).tap do |w|
      @widgets[w.key] = w
    end
  end

  %w(timeline).each do |wid|
    define_method("#{wid}_widget") do |options|
      widget(wid, options)
    end
  end

end