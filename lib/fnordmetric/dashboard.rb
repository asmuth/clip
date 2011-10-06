class FnordMetric::Dashboard

  def initialize(options)
    @options = options.to_options
    raise "please provide a :title" unless @options[:title]
  end

  def title
    @options[:title]
  end
  
end