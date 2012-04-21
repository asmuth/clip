class FnordMetric::Widget

  attr_accessor :gauges, :tick

  def initialize(opts={})
    @opts = opts

    unless opts.has_key?(:title)
      error! "widget can't be initialized without a title"
    end

    add_gauges(opts.delete(:gauges))
  end

  def title
    @opts[:title]
  end

  def token
    token = title.to_s.gsub(/[\W]/, '').downcase
    token = Digest::SHA1.hexdigest(title.to_s) if token.empty?
    token
  end

  def add_gauges(gauges)
    if gauges.blank? && has_tick?
      error! "initializing a widget without gauges is void"
    else
      @gauges = gauges
    end
    
    if (ticks = gauges.map{ |g| g.tick }).uniq.length == 1
      @tick = ticks.first
    elsif !!self.try(:has_tick?)
      error! "you can't add gauges with different ticks to the same widget"
    end
  end

  def error!(msg)
    FnordMetric.error!(msg)
  end

  def range
    ensure_has_tick!
    #@opts[:range] || default_range # FIXME: allow custom ranges, but assure that the range-start is 'on a tick'
    default_range
  end

  def ticks
    ensure_has_tick!
    range.step(@tick)
  end

  def default_range(now=Time.now)
    ensure_has_tick!
    te = gauges.first.tick_at(now.to_i)
    te -= @tick unless include_current?
    rs = (@opts[:ticks] || (@tick == 1.hour.to_i ? 24 : 30)).to_i
    (te-(@tick*rs)..te)
  end

  def include_current?
    !(@opts[:include_current] == false)
  end
    
  def data
    { 
      :title => @opts[:title], 
      :width => @opts[:width] || 100,
      :klass => self.class.name.split("::").last 
    }
  end

  def render
    data
  end

  def ensure_has_tick!
    error! "widget does not have_tick" unless has_tick?
  end

end