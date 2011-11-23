class FnordMetric::Widget

  attr_accessor :gauges, :tick

  def initialize(opts={})
    @opts = opts
    add_gauges(opts.delete(:gauges))
  end

  def title
    @opts[:title]
  end

  def add_gauges(gauges)
    if gauges.blank?
      error! "initializing a widget without gauges is void"
    else
      @gauges = gauges
    end
    
    if (ticks = gauges.map{ |g| g.tick }).length == 1
      @tick = ticks.first
    else
      error! "you can't add gauges with different ticks to the same widget"
    end
  end

  def error!(msg)
    FnordMetric.error!(msg)
  end

  def range
    #@opts[:range] || default_range # FIXME: allow custom ranges, but assure that the range-start is 'on a tick'
    default_range
  end

  def ticks
    range.step(@tick)
  end

  def default_range(now=Time.now)
    te = gauges.first.tick_at(now.to_i)
    te += @tick if include_current?
    rs = @tick == 1.hour.to_i ? 24 : 30
    (te-(@tick*rs)..te)
  end

  def include_current?
    !(@opts[:include_current] == false)
  end
    
  def data
    { :title => @options[:title] }
  end

  def data_json
    data.to_json.gsub('"', '\'')
  end

  def render(elem_id)
    %Q{      
      <script type='text/javascript'>
        FnordMetric.render('#{elem_id}', #{data_json});
      </script>
    }
  end

end