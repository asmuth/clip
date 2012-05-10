class FnordMetric::DummyGauge < FnordMetric::MultiGauge

  def initialize(opts)   
    super(opts)

    @started = Time.now.to_i

    ticks  = [1.day, 1.hour] 
    series = [:fnord, :bar, :fu]

    timeline_widget(
      :tab => "Overview",
      :title => "Totals",
      :ticks => ticks,
      :series => series,
      :height => 350
    ).on(:values_at) do |_series, _ticks, _tick|
      Hash[_ticks.map{ |_t| [_t, 23 + rand(5)] }]
    end

    numbers_widget(
      :tab => "Overview",
      :title => "Total #{key_nouns.last}",
      :series => series,
      :autoupdate => 1
    ).on(:values_for) do |_series|
      {
        :fnord => { :value => 23 + rand(50 * running_since) },
        :fubar => { :value => 23 + rand(50 * running_since) }
      }
    end
  end

  def running_since
    Time.now.to_i - @started
  end

end