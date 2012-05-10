class FnordMetric::NumericGauge < FnordMetric::MultiGauge

  def initialize(opts)   
    super(opts)

    validate_series!
    validate_ticks!

    timeline_widget(
      :tab => "Overview",
      :title => "Total #{key_nouns.last}",
      :ticks => @opts[:ticks],
      :series => @opts[:series],
      :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
      :autoupdate => 10,
      :height => 350
    ).on(:values_at) do |_series, _ticks, _tick|
      series_count_metrics[_series][_tick].values_at(_ticks)
    end

    numbers_widget(
      :tab => "Overview",
      :title => "Total #{key_nouns.last}",
      :series => @opts[:series],
      :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
      :autoupdate => 2
    ).on(:values_for) do |_series|
      render_series_numbers(_series.to_sym)
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

  def incr_series(series, time, value)
    value = value.to_i == 0 ? 1 : value.to_i
    unless series_count_metrics[series]
      return FnordMetric.error("gauge '#{name}' -> unknown series: #{series}")
    end
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

      
  def render_series_numbers(series)
    _t = Time.now.to_i

    {}.tap do |out|
      @opts[:ticks].each do |tick|
        out["#{tick}-now"]  = { 
          :value => series_count_metrics[series][tick].value_at(_t),
          :desc  => "$formatTimeRangePre(#{tick}, 0)"
        }
        out["#{tick}-last"] = { 
          :value => series_count_metrics[series][tick].value_at(_t-tick),
          :desc  => "$formatTimeRangePre(#{tick}, -1)"
        }
      end
    end
  end

end