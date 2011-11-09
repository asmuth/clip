class FnordMetric::Namespace

	@@opts = [:event, :gauge]

	def initialize(key)
	  @handlers = Hash.new([])
	  @key = key
	end

    def announce(event)            
      @handlers[event["_type"]].each{ |c| c.call(event) }
    end

	def method_missing(m, *args, &block)
      raise "unknown option: #{m}" unless @@opts.include?(m)
      send(:"opt_#{m}", *args, &block)
	end

	def opt_event(event_type, opts={}, &block)
	  opts.merge!(:block => block)
	  FnordMetric::Context.new(opts).tap do |context|
	    @handlers[event_type.to_s] << context
	  end      
	end

	def opt_gauge(gauge_key, opts={})
		puts "new gauge: #{gauge_key}"
	end
    
end