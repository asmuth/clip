class FnordMetric::Toplist

  attr_accessor :toplist, :timelines, :total

  def initialize(timeline = {})
    @total   = 0
    @toplist = Hash.new{ |h,k| h[k] = 0 }

    @timelines = Hash.new do |h,k|
      h[k] = Hash.new{ |h,k| h[k] = 0 }
    end    
  end

  def incr_item(time, item, value)
    @toplist[item] += value.to_f
    @timelines[item][time] += value.to_f
    @total += value.to_f
  end

end