class FnordMetric::RemoteGauge

  def name
    raise NotYetImplementedError
  end

  def initialized
    @backend = FnordMetric.backend
    @uuid = "gauge-#{rand(8**64).to_s(36)}"

    FnordMetric.log "gauge '#{name}' started"

  	@backend.subscribe do |message|
      if message["_sender"] != @uuid 
        message["__to_self"] = (message["_channel"] == name.to_s)
        react(message)
      end
    end
  end

  def hangup
    @backend.hangup
    @backend = nil
  end

private

  def react(ev)
    render!       if ev["_class"] == "render_request" && ev["__to_self"]
    discover!(ev) if ev["_class"] == "discover_request"
    process!(ev)  if ev["_class"] == "request"
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