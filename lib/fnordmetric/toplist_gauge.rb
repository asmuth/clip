class FnordMetric::ToplistGauge < FnordMetric::MultiGauge

  class SeriesProxyGauge < FnordMetric::Gauge

    def _value_at(_t)
      redis.zscore(tick_key(_t), @opts[:key]).to_i
    end

    def _values_at(_times, opts = {})
      _times.map{ |_t| _value_at(_t) }
    end

    def key(_append=nil)
      [@opts[:key_prefix], :gauge, :count, tick, _append].flatten.compact.join("-")
    end

  end

  def initialize(opts)
    @cmds = [:observe]

    super(opts)

    @opts[:show_top_n] ||= 7

    if !opts[:ticks].is_a?(Array) || opts[:ticks].size == 0
      raise "numeric_gauge #{opts[:key]}: missing option ticks"
    end

    if opts[:ticks].any?{ |t| !t.is_a?(Fixnum) }
      raise "numeric_gauge #{opts[:key]}: these are not numbers: #{opts[:ticks].inspect}"
    end
  end

  def observe(*args)
    ctx = args.delete_at(0)

    if args.size == 1
      count_gauges.values.each do |gauge|
        ctx.incr_field(gauge, args.first)
      end
    else
      raise "invalid arguments for incr: #{args.inspect}"
    end
  end

  def render
    super.merge(
      :template => render_template(:toplist_gauge),
      :widgets => {
        :total_timeline => FnordMetric::TimelineWidget.new(
          :title => "Top #{@opts[:show_top_n]} #{key_nouns.last}",
          :multi_tick => true,
          :render_target => ".toplgauge_widget_topx_timeline",
          :ticks => @opts[:ticks],
          :_gauges => series_values.map{ |s| "#{name}++series-#{s}" },
          :_gauge_titles => Hash[series_values.map{ |s| ["#{name}++series-#{s}", s] }],
          :include_current => true,
          :height => 350
        ).data,
        :total_toplist => FnordMetric::ToplistWidget.new(
          :title => "Top #{key_nouns.last}",
          :multi_tick => true,
          :render_target => ".toplgauge_widget_topx_toplist",
          :ticks => @opts[:ticks],
          :tick => @opts[:ticks].first,
          :gauges => [count_gauges[@opts[:ticks].first]],
          :_gauges => ["#{name}++count"],
          :key_nouns => key_nouns,
          :height => 350
        ).data
      }
    )
  end

  def fetch_gauge(name, tick)
    if name == "count"
      puts "YEAH: #{tick} #{count_gauges.inspect}"
      count_gauges[tick]
    elsif name.starts_with?("series-")
      series_gauges[name[7..-1].to_sym][tick.to_i]
    end.tap do |gauge|
      gauge.try(:add_redis, @opts[:redis])
    end
  end

private

  def count_gauges
    @count_gauges ||= Hash[@opts[:ticks].map do |tick|
      [tick.to_i, FnordMetric::Gauge.new(
        :key => "count", 
        :key_prefix => key,
        :tick => tick.to_i,
        :three_dimensional => true
      )]
    end]
  end

  def series_values
    biggest_tick = @opts[:ticks].map(&:to_i).sort.last
    rkey = count_gauges[biggest_tick].tick_key(Time.now.to_i)
    res = @opts[:redis].zrevrangebyscore(rkey, "+inf", 0, :limit => [0, @opts[:show_top_n]]) 
    res.map(&:to_sym)
  end

  def series_gauges
    Hash.new do |h, k|
      h[k] = Hash[@opts[:ticks].map do |_t|
        [_t.to_i, SeriesProxyGauge.new(
          :key => k,
          :key_prefix => key,
          :tick => _t.to_i
         )]
      end]
    end
  end

end