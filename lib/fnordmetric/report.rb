class FnordMetric::Report

  attr_accessor :events, :metrics

  def initialize(_metrics, options)
    @options = options
    @metrics = Hash.new
    @events = Array.new
    _metrics.each{ |k,m| self.add_metric!(m) }
  end

  def add_metric!(metric)
    @metrics[metric.token] = metric
    add_helper_methods(metric)
  end

  def metaclass
    class << self; self; end
  end

private

  def add_helper_methods(metric)
    self.metaclass.send(:define_method, metric.token) do 
      @metrics[metric.token]
    end
  end

end