class FnordMetric::Report

	attr_accessor :nodes

	def initialize(klass, options)
		@klass = klass
		@options = options
		@nodes = Array.new
	end

end