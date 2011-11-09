class FnordMetric::Namespace

	@@opts = [:event]

	def initialize(key)
	  @handlers = {}
	  @key = key
	end

    def announce(event)
      #puts "<#{@key}> #{event}"
      return if !event["_type"]
      if (handler=@handlers[event["_type"]])
      	handler.call(event)
      end      
    end

	def method_missing(m, *args, &block)
      raise "unknown option: #{m}" unless @@opts.include?(m)
      send(:"opt_#{m}", *args, &block)
	end

	def opt_event(event_type, &block)
	  @handlers[event_type.to_s] = block
      puts "event registered: #{event_type}"
	end
    
end