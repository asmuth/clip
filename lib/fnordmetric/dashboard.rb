class FnordMetric::Dashboard

  attr_accessor :widgets

  def initialize(options={})    
    raise "please provide a :title" unless options[:title]        
    @widgets = Array.new
    @options = options
  end

  def add_widget(w)
    #@widgets << (w.is_a?(FnordMetric::Widget) ? w : FnordMetric.widgets.fetch(w))
  end

  def title
    @options[:title]
  end

  def token
    title.to_s.gsub(/[\W]/, '')
  end
  
end