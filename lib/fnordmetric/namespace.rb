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
	  opts.merge!(:key => gauge_key, :gauge_key => @opts[:gauge_key])
	  @gauges[gauge_key] = FnordMetric::Gauge.new(opts)		
	end
    
end