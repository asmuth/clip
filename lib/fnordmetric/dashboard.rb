class FnordMetric::Dashboard

  attr_accessor :widgets

  def initialize(options, &block)
    @options = options.to_options
    raise "please provide a :title" unless @options[:title]
    @widgets = Array.new
    block.call(self)
  end

  def widget(metric_names, options)
    options.merge!(:metric_names => metric_names)
    raise "missing option: :type" unless options[:type]
    klass = if FnordMetric::WIDGET_TYPES.include?(options[:type].to_s) 
      "FnordMetric::#{options[:type].to_s.classify}Widget".constantize
    else
      raise "unknown widget type: #{options[:type]}"
    end
    @widgets << klass.new(options)
  end

  def title
    @options[:title]
  end

  def token
    title.gsub(/[\W]/, '')
  end
  
end