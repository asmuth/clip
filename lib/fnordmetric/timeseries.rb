class FnordMetric::Timeseries

  def initialize(timeline = {})
    @timeline = Hash.new{ |h,k| h[k] = [0,nil] }
    @timeline.merge!(timeline)
  end

  def incr_fraction(time, numerator, denominator)
  	incr_numerator(time, numerator) if numerator
  	incr_denominator(time, denominator) if denominator
  end

  def incr_numerator(time, value)
  	@timeline[time.to_i][0] += value
  end

  def incr_denominator(time, value)
    @timeline[time.to_i][-1] ||= 0
    @timeline[time.to_i][-1] += value
  end

  def timeseries(range, window, &block)
    res = Hash.new{ |h,k| h[k] = [0,0] }

    (((range.size)/window.to_f).ceil+1).times.map do |n| 
      res[((range.first+window*(n-1))/window.to_f).floor*window] = [0,0]
    end

    @timeline.each do |time, vals|
      next unless range.include?(time)
      wtime = (time/window.to_f).floor * window
      if block
        res[wtime] = block.call(*vals)
      else
        res[wtime][0] += vals[0]
        res[wtime][1] += vals[1]
      end
    end
    
    FnordMetric::Timeseries.new(res)
  end

  def sum(range = (ticks.first..ticks.last))
    @timeline
      .inject(0){ |s,(t,v)| s + (range.include?(t) ? value_at(t) : 0) }
  end

  def trend(range = (ticks.first..ticks.last))
    range ||= (ticks.first..ticks.last)    

    rvals = @timeline.to_a
      .select{ |t,v| range.include?(t) }
      .sort{ |a,b| a.first <=> b.first }
      .map{ |t,v| value_at(t) }

    return 0 if rvals.size == 0
    (rvals.last - rvals.first).to_f / rvals.first
  end

  def ticks
    @timeline.keys.sort
  end

  def value_at(time)
    if @timeline[time][1].to_i > 0
      @timeline[time][0] / @timeline[time][1].to_f
    else
      @timeline[time][0]
    end    
  end

  def to_json(&block)
    @timeline.to_a
      .sort{ |a,b| a[0] <=> b[0] }
      .map { |t,v| { :x => t, :y => block.call(*v), :v0 => v[0], :v1 => v[1] } }
      .to_json
  end

end