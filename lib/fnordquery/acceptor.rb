class FnordQuery::Acceptor

  def initialize(opts)
    @opts = opts

    inbound_class = if @opts[:protocol] == :udp 
      FnordQuery::UDPAcceptor
    else
      FnordQuery::TCPAcceptor
    end

    @opts[:listen] = [
      @opts[:host] || "0.0.0.0",
      @opts[:port] || 2323
    ]

    begin
      inbound_stream = inbound_class.start(@opts)
      FnordQuery.log "listening on #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}"
    rescue
      FnordQuery.log "cant start #{inbound_class.name}. port in use?"
    end
  end

end