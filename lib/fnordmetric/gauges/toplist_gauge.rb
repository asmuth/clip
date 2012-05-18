class FnordMetric::ToplistGauge < FnordMetric::Gauge

  def render(namespace, event)
    interval = parse_interval(event["interval"])
    colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]

    #res = @opts[:redis].zrevrangebyscore(rkey, "+inf", 0, :limit => [0, @opts[:show_top_n]]) 

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
    ctx.redis_exec :zremrangebyrank, tick_key(at, :toplist), 0, -top_k
  end

  def top_k
    (@opts[:top_k] || 1000).to_i
  end

end