class FnordMetric::Web

  def initialize(opts)
    @opts = opts

    @opts[:server] ||= "thin"
    @opts[:host]   ||= "0.0.0.0"
    @opts[:port]   ||= "4242"

    FnordMetric.register(self)
  end

  def initialized   
    server = @opts[:server].downcase

    websocket = FnordMetric::WebSocket.new
    webapp    = FnordMetric::App.new(@opts)

    dispatch  = Rack::Builder.app do
      use Rack::CommonLogger
      use Rack::ShowExceptions

      map("/stream"){ run websocket }
      map("/"){ run webapp }
    end

    unless ["thin", "hatetepe"].include? server
      raise "Need an EventMachine webserver, but #{server} isn't"
    end

    host = @opts[:host] 
    port = @opts[:port]

    Rack::Server.start(
      :app => dispatch,
      :server => server,
      :Host => host, 
      :Port => port
    ) && FnordMetric.log("listening on http://#{host}:#{port}")
  end

end