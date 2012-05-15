class FnordQuery::Web

  def initialize(opts)
    @opts = opts

    listen = @opts[:listen]

    @opts[:server] ||= @opts[:web_server] || "thin"
    @opts[:host]   ||= (listen ? listen[0] : nil) || "0.0.0.0"
    @opts[:port]   ||= (listen ? listen[1] : nil) || "4242"
  end

  def execute(runner, backend)
    server = @opts[:server].downcase
    app = FnordQuery::Web::App.new(@opts)

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
    ) && puts("listening on http://#{host}:#{port}")      
  end

end