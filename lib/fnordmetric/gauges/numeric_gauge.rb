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
      Hash[_ticks.map{ |_t| [_t, 23] }]
    end

  end

  def incr(*args)
    ctx = args.delete_at(0)

    if args.size == 0 || (args.size == 1 && args.first.is_a?(Fixnum))
      incr_series(ctx, *args.unshift(:_default))
    elsif args.size == 1 || (args.size == 2 && args.last.is_a?(Fixnum))
      incr_series(ctx, *args)
    else
      raise "invalid arguments for incr: #{args.inspect}"
    end
  end

  def incr_series(ctx, series, value = 1)
    if (series == :_default) && @opts[:series].size > 1
      raise "don't know which series to increment - available: #{series}"
    elsif series == :_default
      series = @opts[:series].first
    end

    unless @opts[:series].include?(series)
      raise "unknown series: #{series}"
    end

    series_count_gauges[series].values.each do |gauge|
      ctx.incr(gauge, value)
    end
  end

private

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