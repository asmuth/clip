class FnordMetric::FyrehoseAcceptor

  def self.start(opts)
    require "fyrehose"
    require "fyrehose/reactor"

    new(opts)
  end

  def initialize(opts)
    reactor = EM.connect(opts[:host], opts[:port], Fyrehose::Reactor)

    reactor.on_message do |channel, data|
      event = JSON.parse(data)
      event["_type"] ||= channel
      events << event
      push_next_event
    end

    opts[:channels].each do |channel|
      reactor.subscribe(channel)
    end
  end

  def push_next_event
    return true if events.empty?
    api.event(@events.pop)
    EM.next_tick(&method(:push_next_event))
  end

  def events
    @events ||= []
  end

  def api
    @api ||= FnordMetric::API.new(FnordMetric.options)
  end

  def self.outboud?
    true
  end

end
