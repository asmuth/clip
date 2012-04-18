class FnordMetric::InboundDatagram < EventMachine::Connection

  class << self
    attr_accessor :opts
  end

  def self.start(opts)
    self.opts = opts
    EM.open_datagram_socket(*opts[:inbound_stream], self, opts)
  end

  def receive_data(data)
    api.event(data)
  end

  def api
    @api ||= FnordMetric::API.new(self.class.opts)
  end

end