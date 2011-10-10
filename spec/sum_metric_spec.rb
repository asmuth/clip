require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::SumMetric do

  before(:each) do
    FnordMetric::Event.destroy_all
    FnordMetric.track('my_event_type', :time => 33.hours.ago, :myval => 4)
    FnordMetric.track('my_event_type', :time => 32.hours.ago, :myval => 2)
    FnordMetric.track('my_event_type', :time => 28.hours.ago, :myval => 9)
    FnordMetric.track('my_event_type', :time => 27.hours.ago, :myval => 1)
    FnordMetric.track('my_event_type', :time => 26.hours.ago, :myval => 6)
    FnordMetric.track('my_event_type', :time => 13.hours.ago, :myval => 3)
    FnordMetric.track('my_event_type', :time => 12.hours.ago, :myval => 8)
    FnordMetric.track('my_event_type', :time => 11.hours.ago, :myval => 7)
  end

  it "should sum events until now" do
    metric = FnordMetric.metric('my_event_count', :sum => :myval, :types => [:my_event_type])
    metric.current.should == 40
  end
  
  it "should sum events until 18 hours ago" do
    metric = FnordMetric.metric('my_event_count', :sum => :myval, :types => [:my_event_type])
    metric.at(18.hours.ago).should == 22
  end

  it "should sum events from 30 to 20 hours ago" do
    metric = FnordMetric.metric('my_event_count', :sum => :myval, :types => [:my_event_type])
    metric.at(30.hours.ago..20.hours.ago).should == 16
  end


end