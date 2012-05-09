class FnordMetric::RemoteGauge

  def initialize(opts)
    @backend = FnordMetric.backend
    @uuid = "gauge-#{rand(8**64).to_s(36)}"

  	@backend.subscribe do |message|
      if message["_sender"] != @uuid && message["_channel"] == name.to_s
        react(message)
      end
    end
  end

  def name
    raise NotYetImplementedError
  end

  def react(event)
    raise NotYetImplementedError
  end

  def hangup
    @backend.hangup
    @backend = nil
  end

private

  def respond(message)
  	message["_channel"] ||= name
    @backend.publish(message)
  	#FnordMetric.firehose.push(message)
  end

end