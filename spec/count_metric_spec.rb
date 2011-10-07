require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::CountMetric do

  before(:each) do
    FnordMetric::Event.destroy_all
    FnordMetric.track('my_event_type', :time => 33.hours.ago)
  	FnordMetric.track('my_event_type', :time => 32.hours.ago)
  	FnordMetric.track('my_event_type', :time => 28.hours.ago)
  	FnordMetric.track('my_event_type', :time => 27.hours.ago)
  	FnordMetric.track('my_event_type', :time => 26.hours.ago)
  	FnordMetric.track('my_event_type', :time => 13.hours.ago)
  	FnordMetric.track('my_event_type', :time => 12.hours.ago)
  	FnordMetric.track('my_event_type', :time => 11.hours.ago)
  end

  it "should count events until now" do
  	metric = FnordMetric.metric('my_event_count', :count => true, :types => [:my_event_type])
  	metric.current.should == 8
  end
  
  it "should count events until 18 hours ago" do
  	metric = FnordMetric.metric('my_event_count', :count => true, :types => [:my_event_type])
  	metric.at(18.hours.ago).should == 5
  end

  it "should count events from 30 to 20 hours ago" do
  	metric = FnordMetric.metric('my_event_count', :count => true, :types => [:my_event_type])
  	metric.at(30.hours.ago..20.hour.ago).should == 3
  end

end