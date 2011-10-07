require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Metric do

  before(:each) do
    FnordMetric::Event.destroy_all
    FnordMetric.track('my_event_type', :time => 33.hours.ago, :fnord => "a")
    FnordMetric.track('my_event_type', :time => 32.hours.ago, :fnord => "b")
    FnordMetric.track('my_event_type', :time => 28.hours.ago, :fnord => "c")
    FnordMetric.track('my_event_type', :time => 27.hours.ago, :fnord => "d")
    FnordMetric.track('my_event_type', :time => 26.hours.ago, :fnord => "e")
    FnordMetric.track('another_event_type', :time => 26.hours.ago, :fnord => "x")
    FnordMetric.track('my_event_type', :time => 13.hours.ago, :fnord => "f")
    FnordMetric.track('my_event_type', :time => 12.hours.ago, :fnord => "g")
    FnordMetric.track('my_event_type', :time => 11.hours.ago, :fnord => "h")
  end

  it "should find events by event_type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type, :another_event_type])
    metric.events.count.should == 9
  end

  it "should find events by event_type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type])
    metric.events.count.should == 8
  end

  it "should find events by event_type" do
    metric = FnordMetric::Metric.new(:types => [:another_event_type])
    metric.events.count.should == 1
  end

  it "should find events by time range" do
    metric = FnordMetric::Metric.new({})
    metric.events_at(30.hours.ago..20.hours.ago).count.should == 4
  end

  it "should find events by time range and event type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type, :another_event_type])
    metric.events_at(30.hours.ago..20.hours.ago).count.should == 4
  end

  it "should find events by time range and event type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type])
    metric.events_at(30.hours.ago..20.hours.ago).count.should == 3
  end

  it "should find events until time" do
    metric = FnordMetric::Metric.new({})
    metric.events_at(20.hours.ago).count.should == 6
  end

  it "should find events until time and event type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type, :another_event_type])
    metric.events_at(20.hours.ago).count.should == 6
  end

  it "should find events until time and event type" do
    metric = FnordMetric::Metric.new(:types => [:my_event_type])
    metric.events_at(20.hours.ago).count.should == 5
  end

end