class FnordMetric::Acceptor

  def initialize(opts)
    @opts = opts

    FnordMetric.register(self)
  end

  def initialized
    inbound_class = if @opts[:protocol] == :udp
      FnordMetric::UDPAcceptor
    elsif @opts[:protocol] == :tcp
      FnordMetric::TCPAcceptor
    elsif @opts[:protocol] == :fyrehose
      FnordMetric::FyrehoseAcceptor
    else
      raise "unknown protocol: #{@opts[:protocol]}"
    end

    @opts[:listen] = [
      @opts[:host] || "0.0.0.0",
      @opts[:port] || 2323
    ]

    begin
      inbound_stream = inbound_class.start(@opts)
      FnordMetric.log "listening on #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}"
    rescue Exception => e
      raise e if ENV["FNORDMETRIC_ENV"] == "dev"
      FnordMetric.log "cant start #{inbound_class.name} on #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}. port in use?"
    end
  end

end
