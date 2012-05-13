class FnordQuery::ReportManager

  attr_accessor :opts

  def self.load(opts)
    {}.tap do |reports|
      if File.exists?(reports_dir(opts))
        Dir.foreach(reports_dir(opts)).each do |fn|
          next unless m = fn.match(/^report_(.*)\.json$/)
          fc = File.read(File.join(reports_dir(opts), fn))
          reports[m[1]] = self.new(m[1], JSON.parse(fc).merge(opts))
        end
      end
    end
  end

  def initialize(token, _opts)
    @opts = _opts
    @opts.merge!("token" => token)

    @opts[:available_intervals] = {}

    if File.exists?(report_dir)
      Dir.foreach(report_dir).each do |fn|
        next unless fn.match(/^[0-9]+-[0-9]+$/)
        @opts[:available_intervals].merge!(fn => true)
      end     
    end
  end

  def interval_available?(interval)
    @opts[:available_intervals].keys.include?(interval)
  end

  def token
    @opts["token"]
  end

  def title
    @opts["token"]
  end

  def to_json
    @opts.to_json
  end

#private
   
  def reports_dir(*args)
    self.class.reports_dir(@opts, *args)
  end

  def report_dir(*append)
    File.join(*([@opts[:path], "data", "report_#{@opts["token"]}", append].compact))
  end

  def self.reports_dir(_opts, *append)
    File.join(*([_opts[:path], "config", append].compact))
  end

end