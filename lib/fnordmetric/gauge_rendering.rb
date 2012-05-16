module FnordMetric::GaugeRendering

  def renderable?
    false
  end

  def render_to_event(*args)
    { :title => name }.merge(render(*args))
  end

  def render_haml(in_file)
    haml_engine = Haml::Engine.new(File.read(
      File.expand_path("../../../web/haml/#{in_file}.haml", __FILE__)
    )).render(binding) 
  end

  def render_file(in_file)
    File.read(
      File.expand_path("../../../web/#{in_file}", __FILE__)
    )
  end

end