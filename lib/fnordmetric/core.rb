module FnordMetric

  WIDGET_TYPES = %(graph funnel)
  
  @@metrics = {}
  @@widgets = {}
  @@dashboards = Array.new

  def self.define(metric_name, options)
    warn "FnordMetric.define is deprecated, please use FnordMetric.metric instead"
    self.metric(metric_name, options)
  end

  def self.metric(metric_name, options)
    options.merge!(:name => metric_name)
    @@metrics[metric_name] = FnordMetric::Metric.from_options(options)
  end

  def self.widget(widget_name, options)
    options.merge!(:widget_name => widget_name)
    raise "missing option: :type" unless options[:type]
    klass = if FnordMetric::WIDGET_TYPES.include?(options[:type].to_s) 
      "FnordMetric::#{options[:type].to_s.classify}Widget".constantize
    else
      raise "unknown widget type: #{options[:type]}"
    end
    @@widgets[widget_name] = klass.new(options)
  end

  def self.dashboard(title, options={}, &block)
    options.merge!(:title => title)
    @@dashboards << FnordMetric::Dashboard.new(options, block)
  end

  def self.track(event_name, event_data)
    FnordMetric::Event.track!(event_name, event_data)
  end

  def self.report(options)  
    FnordMetric::Report.new(metrics, options)
  end
  
  def self.metrics
    @@metrics
  end

  def self.widgets
    @@widgets
  end

  def self.reset_metrics
    @@metrics = {}
  end

  def self.reset_widgets
    @@widgets = {}
  end

  def self.dashboards
    @@dashboards
  end

end
