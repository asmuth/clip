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
      series_count_metrics[_series][_tick].values_at(_ticks)
    end

    timeline_widget(
      :tab => "Overview 2",
      :title => "Totals Random",
      :ticks => @opts[:ticks],
      :series => @opts[:series],
      :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
      :height => 350
    ).on(:values_at) do |_series, _ticks, _tick|
      series_count_metrics[_series][_tick].values_at(_ticks)
    end

  end

  def react(event)
    if event["_class"] == "incrby" || event["_class"] == "incr"
      series = event["series"]
      series ||= @opts[:series][0] if @opts[:series].size == 1
      incr_series(series.to_sym, event["_time"], event["value"])
    end
  end

private

  def incr_series(series, time, value = 1)
    series_count_metrics[series].values.each do |metric|
      metric.incr(time, value)
    end
  end

  def series_count_metrics
    @series_gauges ||= Hash[@opts[:series].map do |series|
      [series, Hash[@opts[:ticks].map do |tick|
        [tick.to_i, FnordMetric::RedisMetric.new(
          :key => "count-#{series}", 
          :key_prefix => key,
          :tick => tick.to_i,
        )]
      end]]
    end]
  end

end