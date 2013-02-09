class FnordMetric::TCPAcceptor < EventMachine::Connection
  @@opts = nil

  def self.start(opts)
    @@opts = opts
    EM.start_server(*(opts[:listen] + [self]))
  end

  def self.options(opts)
    @@opts = opts
  end

  def receive_data(chunk)
    @buffer << chunk
    next_event
  end

  def next_event
    read_next_event
    push_next_event
  end

  def read_next_event
    while (event = @buffer.slice!(/^(.*)\n/))
      @events_buffered += 1
      @events << event
    end
  end

  def push_next_event
    return true if @events.empty?
    @events_buffered -= 1
    api.event(@events.pop)
    close_connection?
    EM.next_tick(&method(:push_next_event))
  end

  def close_connection?
    #@backend.hangup unless @streaming || (@events_buffered!=0)
  end

  def post_init
    @events_buffered = 0
    @streaming = true
    @buffer = ""
    @events = []
  end

  def unbind
    @streaming = false
    close_connection?
  end

  def api
    @api ||= FnordMetric::API.new(FnordMetric.options)
  end

end
