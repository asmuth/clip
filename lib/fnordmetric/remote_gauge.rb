class FnordMetric::RemoteGauge

  def initialize(opts)
  	FnordMetric.chan_upstream.subscribe do |message|
  	  react(message) if message["_channel"] == name.to_s
    end
  end

  def name
    raise NotYetImplementedError
  end

  def react
    raise NotYetImplementedError
  end

private

  def respond(message)
  	message["_channel"] ||= name
  	FnordMetric.chan_upstream.push(message)
  end

end