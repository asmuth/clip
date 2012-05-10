class FnordMetric::RealtimeValueWidget < FnordMetric::Widget
  
  def react(ev)
    puts ev.inspect
  end  

end