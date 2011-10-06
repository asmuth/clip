class FnordMetric::Widget

  attr_accessor :report

  def initialize(options={})
  	@options = options
  	add_report(@options[:report]) if @options[:report]
  end

  def title
  	@options[:title]
  end

  def metrics
  	raise "please add a report first" unless @report
  	@report.metrics.map{|k,v| v }
  end
    
  def add_report(report)
    @report = report
  end

end