class FnordMetric::GraphWidget < FnordMetric::Widget

  def data
    super.merge(
      :x_labels => ticks.map{ |t| build_label(t) },      
      :series => metrics.map{ |m| build_series(m) },
      :widget_url => "/widget_graph.js?#{(rand*999).to_i}"
    )
  end

private

  def build_label(tick)
    tick.first.strftime("%d.%m")
  end

  def build_series(metric)
    {  
      :name => metric.token,     
      :data => ticks.map{ |tick| 
        metric.at(@options[:delta] ? tick : tick.last)
      }
    }
  end


end