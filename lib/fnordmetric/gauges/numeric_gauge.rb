class FnordMetric::NumericGauge < FnordMetric::MultiGauge

  def initialize(opts)   
    super(opts)

    validate_series!
    validate_ticks!

    @overview_timeline = timeline_widget(
      :tab => "Overview",
      :title => "Totals",
      :ticks => @opts[:ticks],
      :series => @opts[:series],
      :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
      :height => 350
    )

    @overview_timeline.on(:values_at) do |_series, _ticks, _tick|
      series_count_gauges[_series][_tick].values_at(_ticks)
    end

  end

  def react(event)
    if event["_class"] == "incrby"
      series = event["series"]
      series ||= @opts[:series][0] if @opts[:series].size == 1
      incr_series(series.to_sym, event["_time"], event["value"])
    end
  end

private

  def incr_series(series, time, value = 1)
    series_count_gauges[series].values.each do |gauge|
      gauge.incr(time, value)
    end
  end

  def series_count_gauges
    @series_gauges ||= Hash[@opts[:series].map do |series|
      [series, Hash[@opts[:ticks].map do |tick|
        [tick.to_i, FnordMetric::Gauge.new(
          :key => "count-#{series}", 
          :key_prefix => key,
          :tick => tick.to_i,
        )]
      end]]
    end]
  end

end