class FnordMetric::MemoryBackend

  def initialize(opts)
    @subscriptions = []
  end

  def subscribe(&block)
    sid = channel.subscribe do |message|
      block.call(message)
    end

    @subscriptions.push(sid)
  end

  def publish(message)
    channel.push(message)
  end

  def hangup
    @subscriptions.each do |sid|
      channel.unsubscribe(sid)
    end
  end

private

  def channel
    $fm_channel ||= EM::Channel.new
  end

end