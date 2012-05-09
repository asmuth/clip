class FnordMetric::RemoteGauge

  def name
    raise NotYetImplementedError
  end

  def initialized
    @backend = FnordMetric.backend
    @uuid = "gauge-#{rand(8**64).to_s(36)}"

    FnordMetric.log "gauge '#{name}' started"

  	@backend.subscribe do |message|
      _react(message)
    end
  end

  def hangup
    @backend.hangup
    @backend = nil
  end

private

  def _react(ev)
    puts ev.inspect
    return discover!(ev) if ev["_class"] == "discover_request"
    react(ev) if (ev["_channel"] == name.to_s) && ev["_sender"] != @uuid
  end

  def discover!(event)
    respond(
      :_channel  => event["_channel"],
      :_class    => "discover_response",
      :gauge_key => name
    )
  end

  def respond(message)
  	message["_channel"] ||= name
    message["_sender"] ||= @uuid
    @backend.publish(message)
  end

end