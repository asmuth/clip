class FnordMetric::Histogram < Hash

  def initialize
    super{ |h,k| h[k]=0 }
  end

  def set_opts(opts = {})
    @opts = opts
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
    windows = histogram_windows(windows) unless windows.is_a?(Array)
    Hash[windows.map{ |w| [w,0] }].tap do |histo|
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
      [r.size == 1.0 ? r.last.to_s : json_value(r), v.to_i]
    end.to_json
  end

  def json_value(r)
    "#{r.first.round(@opts[:precision]).to_s}-#{r.last.round(@opts[:precision]).to_s}"
  end

private

  def histogram_windows(windows)
    _min = min
    _max = max

    return [(0..1)] if (_max-_min == 0)

    windows.times
      .inject((_min.._max)
      .step(((_max-_min)/windows.to_f)).to_a << _max){ |a,n|
        a[n]=(a[n]..a[n+1]); a }.take(windows)
  end

end
