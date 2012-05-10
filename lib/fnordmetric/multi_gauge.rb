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
    [FnordMetric.options[:redis_prefix], "multigauge", name, _append].flatten.compact.join("-")
  end

  def _react(ev)
    return render_self!       if ev["_class"] == "render_request"
    return render_widget!(ev) if ev["_class"] == "widget_request"
    react(ev)
  end

private

  def render_self!
    payload = {
      :title => title,
      :gauge_key => name,
      :widgets => Hash[@widgets.map{ |k,w|
        [k, w.opts.merge(:channel => name)]
      }]
    }

    respond(
      :_class => "render_response", 
      :payload => payload
    )
  end

  def render_widget!(ev)
    if @widgets.has_key?(ev["widget_key"])
      respond(@widgets[ev["widget_key"]].react(ev).merge(
        :_class => "widget_response",
        :widget_key => ev["widget_key"]
      ))
    end
  end

  def widget(klass, opts)
    opts[:gauge_key] ||= name

    [
      "FnordMetric",
      "#{klass.to_s.camelize}Widget"
    ].join("::").constantize.new(opts).tap do |w|
      @widgets[w.key] = w
    end
  end

  %w(timeline numbers realtime_value realtime_feed).each do |wid|
    define_method("#{wid}_widget") do |options|
      widget(wid, options)
    end
  end

  def validate_series!
    if !@opts[:series].is_a?(Array) || @opts[:series].size == 0
      raise "#{@opts[:key]}: missing option series"
    end

    if @opts[:series].size != @opts[:series].uniq.size
      raise "#{@opts[:key]}: series are not unique"
    end

    @opts[:series] = @opts[:series].map(&:to_sym)
  end

  def validate_ticks!
    if !@opts[:ticks].is_a?(Array) || @opts[:ticks].size == 0
      raise "#{@opts[:key]}: missing option ticks"
    end

    if @opts[:ticks].any?{ |t| !t.is_a?(Fixnum) }
      raise "#{@opts[:key]}: these are not numbers: #{@opts[:ticks].inspect}"
    end
  end

end