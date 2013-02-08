class FnordMetric::STOMPAcceptor

  def self.start(opts)
    begin
      require "stomp"
    rescue LoadError
      FnordMetric.error("require 'stomp' failed, you need the stomp gem")
      exit 1
    end

    new(opts)
  end

  def initialize(opts)
    @mutex = Mutex.new

    client = Stomp::Client.new(:hosts => [{
      :host => opts[:host],
      :port => opts[:port],
      :passcode => opts[:password],
      :login => opts[:username]}])

    msg_handler = lambda do |topic, msg|
      data = msg.body

      event = begin
        JSON.parse(data)
      rescue
        FnordMetric.log("[STOMP] received invalid JSON: #{data[0..60]}")
      end

      if event
        event["_type"] ||= topic.gsub(/^\/topic\//, '')
        @mutex.synchronize{ events << event }
      end
    end

    opts[:topics].each do |topic|
      client.subscribe(topic){ |data| msg_handler[topic, data] }
    end

    Thread.new do
      client.join
    end

    EM.next_tick(&method(:push_next_event))
  end

  def push_next_event
    nxt = @mutex.synchronize{ events.pop }
    unless nxt
      EM::Timer.new(0.01, &method(:push_next_event))
      return true
    end
    api.event(nxt)
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
