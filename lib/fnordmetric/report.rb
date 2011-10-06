class FnordMetric::Report

  attr_accessor :events, :metrics

  def initialize(metric_options, options)
    @metric_options = metric_options
    @options = options
    @metrics = Hash.new
    @events = Array.new
    build_all!
  end

  def build_all!
    @metric_options.each{ |k,m| self.build!(m) }
  end

  def build!(metric_options)
    metric = build_metric(metric_options)
    @metrics[metric_options[:name]] = metric
    add_helper_methods(metric_options[:name])
  end

  def metaclass
    class << self; self; end
  end

private

  def build_metric(metric_options)
    FnordMetric::Metric.new(metric_options)
  end

  def add_helper_methods(metric_name)
    self.metaclass.send(:define_method, metric_name) do 
      @metrics[metric_name]
    end
  end

end