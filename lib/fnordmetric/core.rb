module FnordMetric
  
  @@metrics = {}
  @@dashboards = Array.new

  def self.define(metric_name, options)
    options.merge!(:name => metric_name)
    @@metrics[metric_name] = options
  end

  def self.dashboard(title, options={}, &block)
    options.merge!(:title => title)
    @@dashboards << FnordMetric::Dashboard.new(options).tap do |_dashboard|
      block.call(_dashboard)
    end
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

  def self.reset_metrics
    @@metrics = {}
  end

  def self.dashboards
    @@dashboards
  end

end
