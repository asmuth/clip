class FnordMetric::RemoteGauge

  def initialize(opts)
  	#FnordMetric.firehose.subscribe do |message|
    #  puts "RECEIVED: #{message.inspect}"
  	#  react(message) if message["_channel"] == name.to_s
    #end
  end

  def name
    raise NotYetImplementedError
  end

  def react(event)
    raise NotYetImplementedError
  end

private

  def respond(message)
  	message["_channel"] ||= name
  	#FnordMetric.firehose.push(message)
  end

end