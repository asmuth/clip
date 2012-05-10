class FnordMetric::NumbersWidget < FnordMetric::Widget

  def react(ev)
    puts "numbers widget #{ev}"

    {
      :cmd => "values_for",
      :series => ev["series"],
      :values => call_handler(:values_for, ev["series"]) 
    }
  end  

end