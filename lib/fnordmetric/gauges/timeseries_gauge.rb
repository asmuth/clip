class FnordMetric::TimeseriesGauge < FnordMetric::Gauge

  def initialize(opts)
    super(opts)
    
    @opts[:series] = @opts[:series].map(&:to_sym)

    if @opts[:calculate]
      unless [:sum, :average, :progressive_sum].include?(@opts[:calculate].to_sym)
        raise "unknown calculate option: #{@opts[:calculate]}"
      end
      @calculate = @opts[:calculate].to_sym
    end 

    @calculate ||= :sum
    @calculate_proc = lambda{ |c,d| d > 0 ? (c/d.to_f).round(2) : c }
  end

  def render(namespace, event)
    @interval = parse_interval(event["interval"])
    colors = FnordMetric::COLORS.dup

    @series = Hash.new
    @zooms  = FnordMetric::TICKS[tick, @interval.size]

    @total = 0

    @opts[:series].each do |series|
      ts = FnordMetric::Timeseries.new

      fraction_values_in(@interval, series).each do |time, frac|
        @total += frac.first # FIXPAUL
        ts.incr_fraction(time, *frac)
      end

      @series[series] = { 
        :color => colors.unshift(colors.pop).first,
        :data => Hash[@zooms.map{ |int| [int, ts.timeseries(@interval, int) ] }],
        :timeseries => ts
      }
    end

    render_page(:timeseries_gauge)
  end

  def execute(cmd, context, *args)
    return incr(context, *args) if cmd == :incr
    return incr_numerator(context, *args) if cmd == :incr_numerator
    return incr_denominator(context, *args) if cmd == :incr_denominator

    FnordMetric.error("gauge '#{name}': unknown command: #{cmd}")
  end

  def renderable?
    true
  end

  def has_series?
    true
  end

private

  def incr(ctx, series_name = :default, value = 1)
    if @calculate == :average
      incr_numerator(ctx, series_name, value)
      incr_denominator(ctx, series_name, 1)
    elsif @calculate == :sum
      incr_numerator(ctx, series_name, value)
    elsif @calculate == :progressive_sum
      incr_numerator(ctx, series_name, value, true)
    end
  end

end

# class FnordMetric::NumericGauge < FnordMetric::MultiGauge

#   def initialize(opts)   
#     super(opts)

#     validate_series!
#     validate_ticks!

#     timeline_widget(
#       :tab => "Overview",
#       :title => "Total #{key_nouns.last}",
#       :ticks => @opts[:ticks],
#       :series => @opts[:series],
#       :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
#       :autoupdate => 10,
#       :height => 350
#     ).on(:values_at) do |_series, _ticks, _tick|
#       series_count_metrics[_series][_tick].values_at(_ticks)
#     end

#     numbers_widget(
#       :tab => "Overview",
#       :title => "Total #{key_nouns.last}",
#       :series => @opts[:series],
#       :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
#       :autoupdate => 2
#     ).on(:values_for) do |_series|
#       render_series_numbers(_series.to_sym)
#     end

#   end

#   def react(event)
#     if event["_class"] == "incrby" || event["_class"] == "incr"
#       series = event["series"]
#       series ||= @opts[:series][0] if @opts[:series].size == 1
#       incr_series(series.to_sym, event["_time"], event["value"])
#     end
#   end




      
#   def render_series_numbers(series)
#     _t = Time.now.to_i

#     {}.tap do |out|
#       @opts[:ticks].each do |tick|
#         out["#{tick}-now"]  = { 
#           :value => series_count_metrics[series][tick].value_at(_t),
#           :desc  => "$formatTimeRangePre(#{tick}, 0)"
#         }
#         out["#{tick}-last"] = { 
#           :value => series_count_metrics[series][tick].value_at(_t-tick),
#           :desc  => "$formatTimeRangePre(#{tick}, -1)"
#         }
#       end
#     end
#   end

# end
