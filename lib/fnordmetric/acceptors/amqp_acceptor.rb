class FnordMetric::AMQPAcceptor

  def self.start(opts)
    begin
      require "amqp"
    rescue LoadError
      FnordMetric.error("require 'amqp' failed, you need the amqp gem")
      exit 1
    end

    new(opts)
  end

  def initialize(opts)
    amqp = AMQP.connect(:host => 'firehose')
    amqp_channel = AMQP::Channel.new(amqp)

    msg_handler = lambda do |channel, data|
      event = begin
        JSON.parse(data)
      rescue
        FnordMetric.log("[AMQP] received invalid JSON: #{data[0..60]}")
      end

      if event
        event["_type"] ||= channel
        events << event
        push_next_event
      end
    end

    opts[:channels].each do |channel|
      queue = amqp_channel.queue(channel, :auto_delete => true)
      queue.subscribe{ |data| msg_handler[channel, data] }
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

  def self.outbound?
    true
  end

end
