class FnordMetric::EventHandler

  def initialize(opts={}, &block)
    @opts = opts
    @block = block
    @uuid = "evhandler-#{rand(8**32).to_s(36)}"
    @channels = []

    FnordMetric.register(self)
  end

  def initialized   
    @backend = FnordMetric.backend

    @backend.subscribe do |event|
      if @channels.include?(event["_channel"]) && event["_sender"] != @uuid
      	@callback.try(:call, event)
      end
    end

    instance_exec &@block
  end

private

  def emit(event)
  	event.merge!("_sender" => @uuid)
  	@backend.publish(event)
  end

  def subscribe(channel)
  	@channels << channel.to_s
  	@channels.uniq!
  end

  def receive(&callback)
  	@callback = callback
  end

end