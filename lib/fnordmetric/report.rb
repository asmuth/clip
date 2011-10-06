class FnordMetric::Report

  attr_accessor :nodes

  def initialize(metric_data, options)
    @metric_data = metrics
    @options = options
    @metrics = Array.new
    @data = Hash.new
    @events = Array.new
    build_all!
  end

  def build_all!
    @metric_data.each{ |k,m| self.build!(m) }
  end

  def build!(metric)
    calculate_metric(metric)
    add_helper_methods(metric)
  end

  def metaclass
    class << self; self; end
  end

private

  def calculate_metric(metric_data)
    @metrics[metric_data[:name]] = 123
  end

  def add_helper_methods(metric_name)
    self.metaclass.send(:define_method, metric_name) do 
      @metrics[metric_name]
    end
  end

end