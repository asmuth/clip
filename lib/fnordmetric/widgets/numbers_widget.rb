class FnordMetric::NumbersWidget < FnordMetric::Widget

  def react(ev)
    {
      :cmd => "values_for",
      :series => ev["series"],
      :values => call_handler(:values_for, ev["series"]) 
    }
  end  

  def has_tick?
    false
  end
  
end