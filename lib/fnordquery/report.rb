class FnordQuery::Report

  def initialize(opts)
  	@opts = opts
  end

  def execute(runner, _backend)
  	%x{mkdir -p #{basedir}}
  end

private

  def render_haml(in_file, out_file)
    in_file = ::File.expand_path("../../../web/haml/#{in_file}.haml", __FILE__)
    haml_engine = Haml::Engine.new(File.read(in_file))
    @render_inner = haml_engine.render(binding)
    layout_file = ::File.expand_path("../../../web/haml/layout.haml", __FILE__)
    haml_engine = Haml::Engine.new(File.read(layout_file))
    File.open(::File.join(basedir, out_file), "w+") do |f|
      f.write(haml_engine.render(binding))
    end
    puts ">> rendered: #{::File.join(basedir, out_file)}"
  end

  def basedir
  	@opts[:basedir]
  end

end