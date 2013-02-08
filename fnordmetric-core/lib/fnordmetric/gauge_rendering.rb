module FnordMetric::GaugeRendering

  def renderable?
    false
  end

  def render_to_event(*args)
    { :title => name }.merge(render(*args))
  end

private

  def render_page(in_file)
    exec_js = []
    content = render_haml(in_file)
    content.scan(/<FNORDMETRIC-GAUGEJS>(.*)<\/FNORDMETRIC-GAUGEJS>/m){ |x| exec_js << x }
    content.gsub!(/<FNORDMETRIC-GAUGEJS>(.*)<\/FNORDMETRIC-GAUGEJS>/m, "")
    { :html => content, :exec => exec_js.flatten * "" }
  end

  def render_haml(in_file)
    haml_engine = Haml::Engine.new(File.read(
      File.expand_path("../../../web/haml/#{in_file}.haml", __FILE__)
    )).render(binding) 
  end

  def parse_interval(interval_str)
    match = interval_str.match(/([0-9]+)-([0-9]+)/)
    raise "invalid interval: #{interval_str}" unless match
    (match[1].to_i..match[2].to_i)
  end

  # FIXPAUL: move to apphelper or something
  def fancy_timerange(range)
    [range.first, range.last].map do |time|
      Time.at(time).strftime("%d.%m.%y %H:%M")
    end
  end

end