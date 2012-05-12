class FnordQuery::Acceptor

  def initialize(opts)
    @opts = opts
  end

  def execute(runner, backend)
    inbound_class = if @opts[:protocol] == :udp
      FnordQuery::UDPAcceptor
    else
      FnordQuery::TCPAcceptor
    end

    @opts[:listen] = [
      @opts[:host] || "0.0.0.0",
      @opts[:port] || 2323
    ]

    @opts.merge!(
      :runner => runner,
      :backend => backend
    )

    begin
      inbound_stream = inbound_class.start(@opts)
      puts "listening on #{@opts[:protocol]}://#{@opts[:listen][0..1].join(":")}"
    #rescue
    #  puts "error: cant start #{inbound_class.name}. port in use?"
    #  exit!(1)
    end
  end

end