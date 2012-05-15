class FnordMetric::UDPClient

  def initialize(host, port)
    @sock = UDPSocket.new
    @sock.connect(host, port)
  end

  def event(event_data)
    begin
      if event_data.is_a?(Hash)
        event_data = event_data.to_json
      else
        MultiJson.decode(event_data) # void ;)
      end
    rescue MultiJson::DecodeError
      FnordMetric.log("event_lost: can't parse json")
    else
      @sock.send(event_data, 0)
    end
  end

end
