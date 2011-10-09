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

  it "#from_options should return an instance of the right subclass" do
    FnordMetric::Metric.from_options(:count => true).should be_a(FnordMetric::CountMetric)
    FnordMetric::Metric.from_options(:sum => :fnord).should be_a(FnordMetric::SumMetric)
    FnordMetric::Metric.from_options(:average => :f).should be_a(FnordMetric::AverageMetric)
    FnordMetric::Metric.from_options(:combine => :l).should be_a(FnordMetric::CombineMetric)
  end

  it "#from_options should raise if none of the mandatory opts is provided" do
    lambda{ FnordMetric::Metric.from_options({}) }.should raise_error(RuntimeError)
  end

  it "should never cache value_at(time) where time is in the future" do
    FnordMetric::Metric.new({}).send(:cache_this?, Time.now.to_i+60).should be_false
  end

  it "should never cache value_at(time) where time is now" do
    FnordMetric::Metric.new({}).send(:cache_this?, Time.now.to_i).should be_false
  end

  it "should always cache value_at(time) where time is in the past" do
    FnordMetric::Metric.new({}).send(:cache_this?, Time.now.to_i-60).should be_true
  end

  it "should never cache value_at(range) where range is completely in the future" do
    range = ((Time.now.to_i+60)..(Time.now.to_i+120))
    FnordMetric::Metric.new({}).send(:cache_this?, range).should be_false
  end

  it "should never cache value_at(range) where range is partially in the future" do
    range = ((Time.now.to_i-60)..(Time.now.to_i+120))
    FnordMetric::Metric.new({}).send(:cache_this?, range).should be_false
  end

  it "should never cache value_at(range) where range ends now" do
    range = ((Time.now.to_i-60)..Time.now.to_i)
    FnordMetric::Metric.new({}).send(:cache_this?, range).should be_false
  end

  it "should always cache value_at(range) where range is completely in the past" do
    range = ((Time.now.to_i-120)..(Time.now.to_i-60))
    FnordMetric::Metric.new({}).send(:cache_this?, range).should be_true
  end

end