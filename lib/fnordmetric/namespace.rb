class FnordMetric::Namespace
  
  attr_reader :handlers, :gauges, :opts, :key

  @@opts = [:event, :gauge, :widget]

  def initialize(key, opts)    
    @gauges = Hash.new
    @dashboards = Hash.new
    @handlers = Hash.new([])                 
    @opts = opts
    @key = key      
  end

  def ready!(redis)
    @redis = redis
    self
  end

  def announce(event)                  
    announce_to_timeline(event)
    announce_to_typelist(event)
    announce_to_session(event) if event[:_session]

    @handlers[event[:_type]].each do |context| 
      context.call(event, @redis) 
    end

    self
  end

  def announce_to_session(event)
    FnordMetric::Session.create(@opts.clone.merge(
      :namespace_key => @key, 
      :namespace_prefix => key_prefix,
      :redis => @redis,
      :event => event
    )) 
  end

  def announce_to_timeline(event)
    timeline_key = key_prefix(:timeline)
    @redis.zadd(timeline_key, event[:_time], event[:_eid])
  end

  def announce_to_typelist(event)
    typelist_key = key_prefix("type-#{event[:_type]}")
    @redis.lpush(typelist_key, event[:_eid])
  end


  def key_prefix(append=nil)
    [@opts[:redis_prefix], @key, append].compact.join("-")
  end

  def token
    @key
  end

  def dashboards(name=nil)
    return @dashboards unless name
    @dashboards[name] ||= FnordMetric::Dashboard.new(
      :title => name
    )       
  end

  def sessions(_ids, opts={})
    return FnordMetric::Session.all(extend_opts(opts)) if _ids == :all
  end

  def events(_ids, opts={})
    return FnordMetric::Event.all(extend_opts(opts)) if _ids == :all
    return FnordMetric::Event.by_type(opts.delete(:type), extend_opts(opts)) if _ids == :by_type
  end

  def method_missing(m, *args, &block)
    raise "unknown option: #{m}" unless @@opts.include?(m)
    send(:"opt_#{m}", *args, &block)
  end

  def opt_event(event_type, opts={}, &block)    
    opts.merge!(:redis => @redis, :gauges => @gauges)   
    FnordMetric::Context.new(opts, block).tap do |context|
      @handlers[event_type.to_s] << context
    end      
  end

  def opt_gauge(gauge_key, opts={})
    opts.merge!(:key => gauge_key, :key_prefix => key_prefix)
    @gauges[gauge_key] = FnordMetric::Gauge.new(opts)   
  end

  def opt_widget(dashboard, widget)
    dashboards(dashboard).add_widget(widget)
  end

  def extend_opts(opts)
    opts.merge(
      :namespace_prefix => key_prefix,
      :redis_prefix => @opts[:redis_prefix],
      :redis => @redis
    )
  end

end
