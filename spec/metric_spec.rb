require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Metric do

  before(:each) do
    FnordMetric::Event.destroy_all
    FnordMetric.track('my_event_type', :time => 33.hours.ago, :fnord => "a")
    FnordMetric.track('my_event_type', :time => 32.hours.ago, :fnord => "b")
    FnordMetric.track('my_event_type', :time => 28.hours.ago, :fnord => "c")
    FnordMetric.track('my_event_type', :time => 27.hours.ago, :fnord => "d")
    FnordMetric.track('my_event_type', :time => 26.hours.ago, :fnord => "e")
    FnordMetric.track('my_event_type', :time => 13.hours.ago, :fnord => "f")
    FnordMetric.track('my_event_type', :time => 12.hours.ago, :fnord => "g")
    FnordMetric.track('my_event_type', :time => 11.hours.ago, :fnord => "h")
    FnordMetric.track('another_event_type', :time => 26.hours.ago, :fnord => "x")
  end

  it "should find events by event_type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type, :another_event_type])
    metric.events.length.should == 9

    metric = FnordMetric::Metric.new(:types => [:my_event_type])
    metric.events.length.should == 8

    metric = FnordMetric::Metric.new(:types => [:another_event_type])
    metric.events.length.should == 1
  end

end