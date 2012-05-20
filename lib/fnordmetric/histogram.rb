class FnordMetric::Histogram < Hash

  def initialize
    super{ |h,k| h[k]=0 }
  end

  def [](key)
  	super(key.to_f)
  end

  def []=(key, val)
  	super(key.to_f, val)
  end

  def min
  	keys.sort.first.to_i
  end

  def max
    keys.sort.last.to_i
  end

  def histogram(windows)
    Hash[histogram_windows(windows).map{ |w| [w,0] }].tap do |histo|
      self.each do |k,v|
        histo.detect do |win, wval|          
          histo[win] += v if win.include?(k)
        end
      end
    end
  end

  def json_histogram(windows)
    histogram(windows).to_a.sort do |a, b|
      a[0].first <=> b[0].first
    end.map do |r, v|
      ["#{r.first.round(1).to_s}-#{r.last.round(1).to_s}", v.to_i]
    end.to_json
  end

private

  def histogram_windows(windows)
    _max = max
    _min = min

    window = (_max - _min) / windows.to_f

    (windows - 1).times.inject([(0..min)]) do |a,w| 
      a << (a[-1].last..a.last.last+window)
    end.tap do |wins|
      wins.delete_at(0)
      wins[-1] = (wins[-1].first.._max)
    end
  end

end
