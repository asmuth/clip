module FnordMetric
  
  @@metrics = {}

  def self.define(metric_name, options)
    options.merge!(:name => metric_name)
    @@metrics[metric_name] = options
  end

  def self.track(event_name, event_data)
  end

  def self.report(options)  
  end
  
  def self.metrics
    @@metrics
  end

end
