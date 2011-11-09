class FnordMetric::Namespace

	@@opts = [:event, :gauge]

	def initialize(key, opts)
	  @opts = opts
	  @gauges = Hash.new
	  @handlers = Hash.new([])	  
	  @redis = opts.delete(:redis)
	  @key = key	  
	end

    def announce(event)            
      @handlers[event["_type"]].each{ |c| c.clone.call(event) }
    end

    def key_prefix
      [@opts[:redis_prefix], @key].join("-")
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
    
end