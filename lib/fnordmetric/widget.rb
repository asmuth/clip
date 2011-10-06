class FnordMetric::Widget

  def initialize(options={})
  	@options = options
  end

  def title
  	@options[:title]
  end

  def metrics
  	raise @options.inspect
  end
 
end