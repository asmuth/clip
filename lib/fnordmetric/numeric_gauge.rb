class FnordMetric::NumericGauge < FnordMetric::MultiGauge

  def initialize(opts)
    @cmds = [:incr]

    super(opts)

    if !opts[:series].is_a?(Array) || opts[:series].size == 0
      raise "numeric_gauge #{opts[:key]}: missing option series"
    end

    if !opts[:ticks].is_a?(Array) || opts[:ticks].size == 0
      raise "numeric_gauge #{opts[:key]}: missing option ticks"
    end

    opts[:series] = opts[:series].map(&:to_sym)

    if opts[:ticks].any?{ |t| !t.is_a?(Fixnum) }
      raise "numeric_gauge #{opts[:key]}: these are not numbers: #{opts[:ticks].inspect}"
    end

    if opts[:series].size != opts[:series].uniq.size
      raise "numeric_gauge #{opts[:key]}: series are not unique"
    end
  end

  def react(event)
    render! if event["_class"] == "render_request"
    process!(event) if event["_class"] == "request"
  end

  def process!(event)
    sleep 2
    resp = if event["widget"] == "total_timeline"
      event.merge(
        :values => Hash[series_count_gauges.map do |_skey, _series|
          gauge = fetch_gauge(_series[event["tick"].to_i])
          vals = {}
          event["ticks"].each{ |_tick| vals[_tick.to_i] ||= 0 } 
          vals.merge!(gauge.values_at(event["ticks"]))
          [_skey, vals]
        end]
      )
    end

    if resp
      resp.merge!(
        "_class" => "response",
        "_sender" => @uuid
      )
      resp.delete("ticks")
      respond(resp)
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

  def render
    super.merge(
      :template => render_template(:numeric_gauge),
      :widgets => {
        :total_timeline => {
          :width => 100,
          :klass => "TimelineWidget",
          :title => "Totals",
          :multi_tick => true,
          :render_target => ".numgauge_widget_total_timeline",
          :ticks => @opts[:ticks],
          :series => @opts[:series],
          :series_titles => Hash[@opts[:series].map{|s| [s, s]}],
          :include_current => true,
          :channel => name,
          :widget_key => "total_timeline",
          :height => 350
        }
      }
    )
  end

  def fetch_gauge(series, tick = nil)
    if series.is_a?(FnordMetric::Gauge)
      series
    elsif series.starts_with?("count-")
      series_count_gauges[series[6..-1].to_sym][tick]
    end.tap do |gauge|
      gauge.try(:add_redis, @opts[:redis])
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