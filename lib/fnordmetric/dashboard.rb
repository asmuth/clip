class FnordMetric::Dashboard

  attr_accessor :widgets, :report

  def initialize(options, _block=nil, &block)
    @options = options.to_options
    @widgets = Array.new
    raise "please provide a :title" unless @options[:title]
    (_block||block).call(self)
    add_report(@options[:report]) if @options[:report]
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
  
  def add_report(report)
    @report = report
    @widgets.each{ |w| w.add_report(report) }
  end

end