class FnordMetric::DistributionGauge < FnordMetric::Gauge

  def render(namespace, event)
    interval = parse_interval(event["interval"])
    colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]

    #@num_min =
    #@num_max =

    @histogram = FnordMetric::Histogram.new
    @values = []

    @mmm_timeseries = Hash.new do |h,k| 
      h[k] = { :min => (1.0/0), :max => 0, :avg => [] }
    end

    tick_keys(interval, :histogram).each do |tkey|
      sync_redis.hgetall(tkey).each do |_val, _count|        
        _count = _count.to_f

        @histogram[_val] += _count
        @values << _count

        if _count < @mmm_timeseries[tkey][:min]
          @mmm_timeseries[tkey][:min] = _count
        end

        if _count > @mmm_timeseries[tkey][:max]
          @mmm_timeseries[tkey][:max] = _count
        end

        @mmm_timeseries[tkey][:avg] << _count
      end

      @mmm_timeseries[tkey][:avg] = @mmm_timeseries[tkey][:avg].average
    end

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