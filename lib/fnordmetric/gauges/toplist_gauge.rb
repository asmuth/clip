class FnordMetric::ToplistGauge < FnordMetric::Gauge

  def render(namespace, event)
    interval = parse_interval(event["interval"])

    #@toplist = FnordMetric::Toplist.new

    ticks_in(interval).each do |_tick|
      puts field_values_at(_tick, 
        :limit => top_k, 
        :append => :toplist
      ).inspect
    end    

    @toplist = {}

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
    puts "YAY"

    at = ctx.send(:time)
    ctx.redis_exec :zincrby, tick_key(at, :toplist), 1, item
    ctx.redis_exec :incrby, tick_key(at, :total), 1
    ctx.redis_exec :zremrangebyrank, tick_key(at, :toplist), 0, -top_k
  end

  def top_k
    (@opts[:top_k] || 1000).to_i
  end

end