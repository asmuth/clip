class FnordMetric::Dashboard

  attr_accessor :widgets, :report

  def initialize(options, _block=nil, &block)
    @options = options.to_options
    @widgets = Array.new
    raise "please provide a :title" unless @options[:title]
    (_block||block).call(self)
    add_report(@options[:report]) if @options[:report]
  end

  def add_widget(w)
    @widgets << (w.is_a?(FnordMetric::Widget) ? w : FnordMetric.widgets.fetch(w))
  end

  def title
    @options[:title]
  end

  def token
    title.to_s.gsub(/[\W]/, '')
  end
  
  def add_report(report)
    @report = report
    @widgets.each{ |w| w.add_report(report) }
  end

end