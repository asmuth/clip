class FnordMetric::ToplistGauge < FnordMetric::Gauge

  def render(namespace, event)
    @interval = parse_interval(event["interval"])

    @toplist = FnordMetric::Toplist.new
    @all_ticks = ticks_in(@interval, tick, 1)

    @all_ticks.each do |_tick|
      field_values_at(_tick, :limit => top_k, :append => :toplist).each do |*args|
        item, count = args.flatten[0..1] # what the fnord... ~paul
        @toplist.incr_item(_tick, item, count)
      end
    end

    @toplist.total = @all_ticks.inject(0){ |s,t| s + sync_redis.get(tick_key(t, :total)).to_i }

    render_page(:toplist_gauge)
  end

  def execute(cmd, context, *args)
    return observe(context, args.first) if cmd == :observe
    FnordMetric.error("gauge '#{name}': unknown command: #{cmd}")
  end

  def renderable?
    true
  end

private

  def observe(ctx, item)
    at = ctx.send(:time)
    ctx.redis_exec :zincrby, tick_key(at, :toplist), 1, item
    ctx.redis_exec :incrby, tick_key(at, :total), 1
    ctx.redis_exec :zremrangebyrank, tick_key(at, :toplist), 0, -top_k
    ctx.redis_exec :expire, tick_key(at, :toplist), retention
  end

  def top_k
    (@opts[:top_k] || 1000).to_i
  end

end
