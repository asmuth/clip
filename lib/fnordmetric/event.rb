class FnordMetric::Event  

  #def self.track!(event_type, event_data)
  #end

  def time
  #  read_attribute(:time).to_i
  end

  def data(key=nil)
  #  send(key)
  end

  alias :[] :data

end