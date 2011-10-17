class FnordMetric::Widget

  attr_accessor :report

  def initialize(options={})
    @options = options
    add_report(@options[:report]) if @options[:report]
  end

  def title
    @options[:title]
  end

  def metrics
    [@options[:metrics]].flatten.map{ |m|
      m.is_a?(FnordMetric::Metric) ? m : FnordMetric.metrics.fetch(m)
    }
  end

  def tick
    @options[:tick] || 1.day
  end

  def range
    @options[:range] || default_range
  end

  def range_to_i
    (range.first.to_i..range.last.to_i)
  end

  def ticks
    range_to_i.step(tick.to_i).map{ |ts|
      te = Time.at(ts+tick.to_i)
      next unless range_to_i.include?(te.to_i) || include_current?
      (Time.at(ts)..Time.at(te))
    }.compact
  end

  def default_range(now=Time.now)
    if tick.to_i == 1.day.to_i
      te = Time.utc(now.year, now.month, now.day+1) 
      (te-30.days)..(te-1.second)
    elsif tick.to_i == 1.hour.to_i
      te = Time.utc(now.year, now.month, now.day, now.hour) 
      (te-24.hours)..(te-1.second)
    else
      (now-(tick*30))..now
    end
  end
    
  def add_report(report)
    @report = report
  end

  def data
    { :title => @options[:title] }
  end

  def data_json
    data.to_json.gsub('"', '\'')
  end

  def include_current?
    !(@options[:current] == false)
  end

  def render(elem_id)
    %Q{      
      <script type='text/javascript'>
        FnordMetric.render('#{elem_id}', #{data_json});
      </script>
    }
  end

end