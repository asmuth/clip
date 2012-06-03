  class FnordMetric::Toplist

  attr_accessor :timelines, :total

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

  def prepare!
    @toplist_arr = @toplist.to_a.sort do |a,b|
      b.last <=> a.last
    end
  end

  def toplist(take = 100)
    prepare! unless @toplist_arr
    @toplist_arr[0..(take-1)]
  end

  def percentage(item)
    (@toplist[item].to_f / total.to_f) * 100.0
  end

  def trend(item)
    times = @timelines[item].keys.sort
    
    ((@timelines[item][times.last] - 
      @timelines[item][times.first]) /
      @timelines[item][times.first])
  end

end