class FnordMetric::Timeseries

  def initialize
    @timeline = Hash.new{ |h,k| h[k] = [0,nil] }
  end

  def incr_dividend(time, value)
  	@timeline[time.to_i].first += value
  end

  def incr_divisor(time, value)
    @timeline[time.to_i].last ||= 0
    @timeline[time.to_i].last += value
  end

end