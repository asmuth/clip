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

private

  def histogram_windows(windows)
    window = (max - min) / windows

    (windows - 1).times.inject([min]) do |a,w| 
      a << a.last + window
    end.map do |swindow|
      (swindow..(swindow+window))
    end    
  end

end