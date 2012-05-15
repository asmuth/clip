class FnordQuery::Report

  def initialize(opts)
  	@opts = opts
  end

  def execute(runner, _backend)
  	%x{mkdir -p #{basedir}}
  end

private

  def render_haml(in_file, out_file)
    puts ">> rendering: #{::File.join(basedir, out_file)}"

    haml_engine = Haml::Engine.new(File.read(
      File.expand_path("../../../web/haml/#{in_file}.haml", __FILE__)
    ))

    File.open(::File.join(basedir, out_file), "w+") do |f|
      f.write(haml_engine.render(binding))
    end    
  end

  def basedir
  	@opts[:basedir]
  end

end