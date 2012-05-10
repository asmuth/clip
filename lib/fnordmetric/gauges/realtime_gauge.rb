class FnordMetric::RealtimeGauge < FnordMetric::MultiGauge

  def initialize(opts)
    super(opts)
    @started = Time.now.to_i

    realtime_value_widget(
      :tab => "Overview",
      :title => "Realtime #{key_nouns.last}"
    )
  end

  def initialized
    super

    timer1 = EventMachine::PeriodicTimer.new(0.01) do
      respond(:_class => "widget_push", :cmd => "value", :value => running_since)
    end
  end

  def react(event)
    if event["_class"] == "observe"
      
    end
  end

  def running_since
    Time.now.to_i - @started
  end

end