class FnordMetric::MetricAPI

  def initialize(params)
    @params = params.to_options   
    @metric = FnordMetric.metrics[params[:name]]
  end

  def render
    return "metric not found" unless @metric
    data = if @params[:at] && @params[:at] =~ /^[0-9]+$/
      { :value => @metric.at(Time.at(@params[:at].to_i)) }
    elsif @params[:at] && @params[:at] =~ /^([0-9]+)-([0-9]+)$/ && @params[:tick]
      { :values => tick_data }
    elsif @params[:at] && @params[:at] =~ /^([0-9]+)-([0-9]+)$/
      { :value => range_data }
    else
      { :value => @metric.at(Time.now) }
    end    
    data.to_json
  end

private

  def range_data
    start_ts, end_ts = @params[:at].split("-").map(&:to_i) 
    @metric.at(start_ts..end_ts)
  end
  
  def tick_data(tick_length=@params[:tick].to_i)
    start_ts, end_ts = @params[:at].split("-").map(&:to_i)    
    ticks = (start_ts..end_ts).step(tick_length)    
    ticks.map do |tick| 
      [tick, @metric.at(@params[:delta] ? (tick..tick+tick_length) : tick)]
    end    
  end

end