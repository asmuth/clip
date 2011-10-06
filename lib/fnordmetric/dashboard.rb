class FnordMetric::Dashboard

  attr_accessor :widgets

  def initialize(options)
    @options = options.to_options
    raise "please provide a :title" unless @options[:title]
    @widgets = Array.new
  end

  def self.widget(metric_names, options)
    options.merge!(:metric_names => metric_names)
    @widgets << FnordMetric::Widget.new(options)
  end

  def title
    @options[:title]
  end

  def token
    title.gsub(/[\W]/, '')
  end
  
end