class FnordMetric::Dispatch

  def self.start!(options)  	
  	raise "missing port" unless options[:port]
  	app = FnordMetric::App.new
  	options[:listen] ||= "127.0.0.1"  	
    Thin::Server.start('0.0.0.0', options[:port], app)
  end

end