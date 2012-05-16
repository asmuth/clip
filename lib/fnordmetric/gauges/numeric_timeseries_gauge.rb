class FnordMetric::NumericTimeseriesGauge < FnordMetric::Gauge

  def render(namespace, event)
    # backend = _backend[0].new(_backend[1])

    # colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]

    # t_since = @opts["since"].to_i
    # t_until = @opts["until"].to_i

    # @tick = (t_until - t_since) / 150

    # @series_timelines = Hash.new
    # @series_queries = Hash.new

    # timeline = Hash[149.times.inject([t_until]) do |a, n|
    #   a + [a.first - n*@tick]
    # end.map do |t|
    #   [t, 0]
    # end]

    # @opts["series"].each do |skey, series|
    #   @opts["series"][skey]["color"] ||= colors.unshift(colors.pop).first
    #   @series_timelines[skey] = timeline.dup
    #   @series_queries[skey] = FnordQuery::Query.new("#{series["query"]}")
    # end

    # query = FnordQuery::Query.new("#{@opts["query"]} since(#{t_since}) until(#{t_until})")

    # puts "tick: #{@tick}"
    # puts "time: #{Time.at(t_since).to_s}-#{Time.at(t_until).to_s}"

    # backend.on_finish do
    #   render_result
    #   runner.send(:shutdown, true)
    # end

    # backend.subscribe(query) do |event|

    #   @series_queries.each do |skey, squery|
    #     if squery.matches?(event)
    #       (event["_time"]-@tick..event["_time"]).to_a.reverse.detect do |t|
    #         if @series_timelines[skey].has_key?(t)
    #           @series_timelines[skey][t] += 1
    #         end
    #       end
    #     end
    #   end

    # end

    {
      :html => render_haml(:numeric_timeseries_gauge),
      :exec => render_file('fnordmetric.numeric_timeseries_gauge.js')
    }
  end

  def renderable?
    true
  end

private

  # def render_result
  #   @series_render = @opts["series"].map do |skey, sopts|
  #     {
  #       :data  => @series_timelines[skey].map { |t,v| 
  #         { :x => t, :y => v } 
  #       }.reverse,
  #       :name  => sopts["name"],
  #       :color => sopts["color"]
  #     }
  #   end
  #  
  # end

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


# private

#   def incr_series(series, time, value)
#     value = value.to_i == 0 ? 1 : value.to_i
#     unless series_count_metrics[series]
#       return FnordMetric.error("gauge '#{name}' -> unknown series: #{series}")
#     end
#     series_count_metrics[series].values.each do |metric|
#       metric.incr(time, value)
#     end
#   end

#   def series_count_metrics
#     @series_gauges ||= Hash[@opts[:series].map do |series|
#       [series, Hash[@opts[:ticks].map do |tick|
#         [tick.to_i, FnordMetric::RedisMetric.new(
#           :key => "count-#{series}", 
#           :key_prefix => key,
#           :tick => tick.to_i,
#         )]
#       end]]
#     end]
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