class FnordMetric::UDPAcceptor < EventMachine::Connection

  class << self
    attr_accessor :opts
  end

  def self.start(opts)
    self.opts = opts

    EM.open_datagram_socket(*(opts[:listen] << self << opts))
  end

  def receive_data(event)
    events << event
    push_next_event
  end

  def push_next_event
    return true if events.empty?
    ev = @events.pop
    api.event(ev)
    EM.next_tick(&method(:push_next_event))
  end

  def unbind
    #backend.hangup
  end

  def events
    @events ||= []
  end

  def api
    @api ||= FnordMetric::API.new(FnordMetric.options)
  end

end
