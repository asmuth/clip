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
    elsif @opts[:protocol] == :amqp
      FnordMetric::AMQPAcceptor
    elsif @opts[:protocol] == :stomp
      FnordMetric::STOMPAcceptor
    else
      raise "unknown protocol: #{@opts[:protocol]}"
    end

    @opts[:listen] = [
      @opts[:host] || "0.0.0.0",
      @opts[:port] || 2323
    ]

    begin
      inbound_stream = inbound_class.start(@opts)
      if inbound_class.respond_to?(:outbound?) && inbound_class.outbound?
        FnordMetric.log "connected to #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}"
      else
        FnordMetric.log "listening on #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}"
      end
    rescue Exception => e
      raise e if ENV["FNORDMETRIC_ENV"] == "dev"
      FnordMetric.log "cant start #{inbound_class.name} on #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}. port in use?"
    end
  end

end
