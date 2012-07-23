class FnordMetric::DistributionGauge < FnordMetric::Gauge

  def render(namespace, event)
    @interval = parse_interval(event["interval"])
    colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]

    @opts[:value_scale] ||= 1
    @opts[:precision]   ||= 1

    #@num_min =
    #@num_max =

    @histogram = FnordMetric::Histogram.new
    @values = []
    @histogram.set_opts(@opts)

    @samples = 0

    @mmm_timeseries = Hash.new do |h,k|
      h[k] = { :min => nil, :max => 0, :avg => [] }
    end

    ticks_in(@interval, tick, 1).each do |_tick|
      tkey = tick_key(_tick, :histogram)

      sync_redis.hgetall(tkey).each do |_val, _count|
        _count = _count.to_f
        _val = _val.to_f * @opts[:value_scale]

        @samples += _count

        @histogram[_val] += _count
        @values += [_val] * _count

        if !@mmm_timeseries[_tick][:min] || (_val < @mmm_timeseries[_tick][:min])
          @mmm_timeseries[_tick][:min] = _val
        end

        if _val > @mmm_timeseries[_tick][:max]
          @mmm_timeseries[_tick][:max] = _val
        end

        @mmm_timeseries[_tick][:avg] += [_val] * _count
      end
    end

    if @opts[:value_ranges]
      @histogram_mode = @opts[:value_ranges]
    else
      @histogram_mode = [23, @histogram.max || 1].min
    end

    @mmm_timeseries_arr = @mmm_timeseries.to_a
      .map{ |k,v| [k, Hash[v.map{ |vk, vv| [vk, (vv.is_a?(Numeric) || vv.is_a?(Array)) ? vv : 0 ] }]] }
      .sort{ |a,b| a.first.to_i <=> b.first.to_i}

    render_page(:distribution_gauge)
  end

  def execute(cmd, context, *args)
    return observe(context, args.first) if cmd == :observe
    FnordMetric.error("gauge '#{name}': unknown command: #{cmd}")
  end

  def renderable?
    true
  end

private

  def observe(ctx, value)
    at = ctx.send(:time)

    if value.is_a?(String) && value.match(/[0-9]+/)
      value = value.to_i
    elsif value.is_a?(String) && value.match(/[0-9]+(\.|,)[0-9]+/)
      value = value.to_f
    end

    unless value.is_a?(Float) || value.is_a?(Fixnum)
      return FnordMetric.error("gauge '#{name}': observe called with non-numerical value: #{value}")
    end

    ctx.redis_exec :hincrby, tick_key(at, :histogram), value.round(2), 1
  end

end
