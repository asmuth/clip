class FnordMetric::InboundDatagram < EventMachine::Connection

  class << self
    attr_accessor :opts
  end

  def self.start(opts)
    self.opts = opts
    EM.open_datagram_socket(*(opts[:inbound_stream] << self << opts))
  end

  def receive_data(event)
    events << event
    push_next_event
  end

  def push_next_event
    return true if events.empty?
    api.event(@events.pop)
    EM.next_tick(&method(:push_next_event))
  end

  def unbind
    api.disconnect
  end

  def api
    @api ||= FnordMetric::API.new(self.class.opts)
  end

  def events
    @events ||= []
  end

end