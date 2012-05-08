class FnordMetric::MultiGauge < FnordMetric::RemoteGauge

  def initialize(opts)
    opts.fetch(:key) && opts.fetch(:key_prefix)
    @opts = opts

    super(opts)
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
    [@opts[:key_prefix], "multigauge", name, _append].flatten.compact.join("-")
  end

  def add_redis(_redis)
    @opts[:redis] = _redis
  end

  def react(event)
    render! if event["_class"] == "render_request"
  end

  def render
    {
      :title => title,
      :template => "--- not yet implemented ---",
      :widgets => {}
    }
  end

  def render!
    respond(
      :_class => "render_response", 
      :payload => render
    )
  end

  def method_missing(method, *args, &block)
    if (m = method.to_s.match(/cmd_([a-zA-Z_]+)/))
      method = m[1]
      unless @cmds.try(:include?, method.to_sym)
        return !!(puts "error: unknown command: #{method}")
      else
        return send(method, *args, &block)
      end
    end
    puts "error: method '#{method}' missing"
    raise NoMethodError.new(method)
  end

private

  def render_template(template_name)
    tpl = File.read(::File.expand_path("../../../haml/#{template_name}.haml", __FILE__))
    Haml::Engine.new(tpl).render(binding)
  end

end