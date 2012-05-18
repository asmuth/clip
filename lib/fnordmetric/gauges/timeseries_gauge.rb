class FnordMetric::TimeseriesGauge < FnordMetric::Gauge

  def render(namespace, event)
    interval = parse_interval(event["interval"])

    # colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]
    # @opts["series"][skey]["color"] ||= colors.unshift(colors.pop).first

    @series_render = series_gauges.map do |series, gauge|
      {
        :name  => series,
        :color => "#0066cc",
        :data  => gauge.values_in(interval).to_a
          .sort{ |a,b| a[0] <=> b[0] }
          .map { |t,v| { :x => t, :y => v||0 } }.reverse
      }
    end

    render_page(:timeseries_gauge)
  end

  def execute(cmd, context, *args)
    incr_series(context, *args) if cmd == :incr
  end

  def renderable?
    true
  end

private

  def incr_series(context, series, value)
    unless series_gauges[series.to_sym]
      return FnordMetric.error("gauge '#{name}': unknown series: #{series}")
    end

    context.incr(series_gauges[series.to_sym], value)
  end

  def series_gauges
    @series_gauges ||= Hash[@opts[:series].map do |series|
      [series, FnordMetric::Gauge.new(
        :key => "count-#{series}", 
        :key_prefix => key,
        :tick => tick.to_i,
      )]
    end]
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