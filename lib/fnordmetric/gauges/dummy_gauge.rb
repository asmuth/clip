class FnordMetric::DummyGauge < FnordMetric::MultiGauge

  def initialize(opts)   
    super(opts)

    ticks  = [1.day, 1.hour] 
    series = [:fnord, :bar, :fu]

    my_widget = timeline_widget(
      :tab => "Overview",
      :title => "Totals",
      :ticks => ticks,
      :series => series,
      :height => 350
    )

    my_widget.on(:values_at) do |_series, _ticks, _tick|
      Hash[_ticks.map{ |_t| [_t, 23 + rand(5)] }]
    end
  end

end