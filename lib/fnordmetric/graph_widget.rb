class FnordMetric::GraphWidget < FnordMetric::Widget

  def data
    super.merge(
      :x_labels => ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],      
      :series => metrics.map{|m| build_series(m) },
      :widget_url => "/widget_graph.js?#{(rand*999).to_i}"
    )
  end

private

  def build_series(metric)
    { 
      :name => metric.token,
      :data => ticks.map{ |tick| metric.at(tick.last) }
    )
  end

end