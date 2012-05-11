class FnordMetric::RealtimeGauge < FnordMetric::MultiGauge

  def initialize(opts)
    super(opts)
    validate_ticks!
    
    @values = {}
    @max_tick = @opts[:ticks].sort.last
    
    @resolution = 10

    realtime_value_widget(
      :tab => "Overview",
      :title => "Realtime #{key_nouns.last}",
      :ticks => @opts[:ticks]
    )

    numbers_widget(
      :tab => "Overview",
      :title => "Realtime #{key_nouns.last}",
      :series => [key_nouns.last],
      :autoupdate => 1,
      :dont_animate => true,
      :width => 30
    ).on(:values_for) do |_series|
      Hash[@opts[:ticks].map do |_tick|
        [_tick, { :desc => "#{_tick}sec avg.", :value => moving_avg(_tick)}]
      end]
    end
  end

  def initialized
    super

    timer1 = EventMachine::PeriodicTimer.new(0.1) do
      respond(
        :_class => "widget_push", 
        :cmd => "value", 
        :values => Hash[@opts[:ticks].map{ |t| [t, moving_avg(t)] }]
      )
    end
  end

  def react(event)
    now = now_ts

    if event["_class"] == "observe"
      @values[now] ||= 0
      @values[now] += 1
    end

    @values.keys.each do |_time|
      @values.delete(_time) if (now - _time) > (@max_tick*@resolution)
    end
  end

private

  def moving_avg(delta, now = now_ts)
    ((0..(delta*@resolution)).inject(0){ |s, d|
      s + @values[now-d].to_i
    } / (delta*@resolution).to_f) * @resolution
  end

  def now_ts
    (Time.now.to_f*@resolution).to_i
  end

end