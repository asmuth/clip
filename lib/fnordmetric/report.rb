class FnordMetric::Report

  attr_accessor :nodes

  def initialize(metrics, options)
    @metrics = metrics
    @options = options
    @data = Hash.new
    @events = Array.new
    build_all!
  end

  def build_all!
    @metrics.each{ |k,m| self.build!(m) }
  end

  def build!(metric)
    calculate_metric(metric)
    add_helper_methods(metric)
  end

  def metaclass
    class << self; self; end
  end

private

  def calculate_metric(metric)
    @data[metric[:name]] = 123
  end

  def add_helper_methods(metric)
    self.metaclass.send(:define_method, metric[:name]) do 
      @data[metric[:name]] 
    end
  end

end