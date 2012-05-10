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
    if event["_class"] == "observe"
      respond(:_class => "widget_push", :cmd => "value", :value => rand(23))
    end
  end

  def running_since
    Time.now.to_i - @started
  end

end