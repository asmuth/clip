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

  def data_for_js
    { :title => @options[:title] }
  end

  def render_js(frame_id="f#{(rand*99999).to_i}")
    %Q{      
      _#{frame_id} = "document.getElementById('#{frame_id}').contentWindow.renderWidget(#{data_for_js.to_json.gsub('"', '\'')});"
      document.write(unescape("%3Ciframe frameborder='none' width='100%' height='400' name='#{frame_id}' id='#{frame_id}' src='/widget.html' onload=%22"+_#{frame_id}+"%22 %3E%3C/iframe%3E"));
    }
  end

  def render
    "<script type='text/javascript'>#{render_js}</script>"
  end


end