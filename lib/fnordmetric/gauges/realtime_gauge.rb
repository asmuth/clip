class FnordMetric::RealtimeGauge < FnordMetric::MultiGauge

  def initialize(opts)   
    super(opts)
    @started = Time.now.to_i

    realtime_value_widget(
      :tab => "Overview",
      :title => "Realtime #{key_nouns.last}"
    )
  end

  def react(event)
    #puts event.inspect
  end

  def running_since
    Time.now.to_i - @started
  end

end