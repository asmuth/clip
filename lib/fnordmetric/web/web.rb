class FnordMetric::Web

  @@namespaces = {}

  def self.namespace(key=nil, &block)
    @@namespaces[key] = block
  end

  def initialize(opts)
    @opts = opts

    @opts[:server] ||= "thin"
    @opts[:host]   ||= "0.0.0.0"
    @opts[:port]   ||= "4242"

    FnordMetric.register(self)
  end

  def initialized   
    server = @opts[:server].downcase
    app = FnordMetric::App.new(@opts)

    unless ["thin", "hatetepe"].include? server
      raise "Need an EventMachine webserver, but #{server} isn't"
    end

    host = @opts[:host] 
    port = @opts[:port]

    Rack::Server.start(
      :app => app,
      :server => server,
      :Host => host, 
      :Port => port
    ) && FnordMetric.log("listening on http://#{host}:#{port}")
        
    FnordMetric::WebSocket.new(
      :host => host, 
      :port => (port.to_i+1)
    ) && FnordMetric.log("listening on ws://#{host}:#{port.to_i+1}")
  end

end