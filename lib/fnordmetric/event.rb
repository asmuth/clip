class FnordMetric::Event  
  include Mongoid::Document

  self.collection_name = 'fnordmetric_event' 

  field :type, :type => String
  field :client, :type => Integer
  field :data, :type => Hash

  def self.track!(event_type, event_data)
    event_data.to_options!
    event_time = (event_data.delete(:time) || Time.now.getutc).to_i
    self.create(:type => event_type, :time => event_time, :data => event_data)
  end

  def time
    read_attribute(:time).to_i
  end

  def [](key)
    send(key)
  end

  def method_missing(method)
    data[method.to_s]
  end

end