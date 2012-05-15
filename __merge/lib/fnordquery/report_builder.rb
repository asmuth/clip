class FnordQuery::ReportBuilder

  def initialize(opts)
    @opts = opts
  end

  def execute(runner, backend)
  	%w(token type).each do |key|
  	  if @opts[key].nil?
  	    puts "error: can't find the #{key} key"
  	    return runner.send(:shutdown)
  	  end
  	end

  	if @opts[:since].nil? || @opts[:until].nil?
  	  puts "error: you have to specify --since and --until"
  	  return runner.send(:shutdown)
  	end

  	begin
  	  t_since = FnordQuery::Query.parse_time(@opts[:since])
  	  t_until = FnordQuery::Query.parse_time(@opts[:until])  	  
  	rescue FnordQuery::Query::InvalidQueryError
  	  puts "error: since/until are not valid times"
  	  return runner.send(:shutdown)
  	end

  	report_path = ::File.join(@opts[:path], "data/report_#{@opts["token"]}/#{t_since}-#{t_until}/")

  	if File.exists?(report_path) && !@opts[:force]
  	  puts "error: #{report_path} already exists, overwrite with --force!"
  	  return runner.send(:shutdown)
  	elsif File.exists?(report_path) && @opts[:force]
  	  puts "overwriting #{report_path}"
  	end

  	%x(mkdir -p #{report_path})

  	File.open(File.join(report_path, "report.json"), "w+") do |jf|
  	  _opts = @opts.dup
  	  _opts.merge!(:since => t_since, :until => t_until)
  	  _opts.merge!(:klass => @opts["type"])
  	  [:force, :path, :basedir, "opt_str", "type"].map{ |k| _opts.delete(k) }
  	  jf.write(_opts.to_json)
  	end

  	report_cmd = "#{$0} #{@opts[:opt_str]*" "} --exec #{File.join(report_path, "report.json")}"

  	puts "running: #{report_cmd}"
  	system(report_cmd)

  	return runner.send(:shutdown)
  end

end