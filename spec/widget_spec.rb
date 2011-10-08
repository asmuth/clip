require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Widget do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  it "should remembe it's own title" do
    widget = FnordMetric::Widget.new(:title => "My Widget")
    widget.title.should == "My Widget"
  end

  it "should add the report on init" do
  	FnordMetric.define(:my_metric, :sum => :my_field)
  	report = FnordMetric.report(:range => (4.days.ago..Time.now))
  	widget = FnordMetric::Widget.new(:report => report)
  	widget.report.should == report
  end
 
  it "should add the report after init" do
  	FnordMetric.define(:my_metric, :sum => :my_field)
  	report = FnordMetric.report(:range => (4.days.ago..Time.now))
  	widget = FnordMetric::Widget.new
  	widget.report.should be_nil
  	widget.add_report(report)
  	widget.report.should == report
  end

  it "should define a new widget when given two metric-token" do
    FnordMetric.define(:first_metric, :count => :true)
    FnordMetric.define(:second_metric, :count => :true)
    widget = FnordMetric::Widget.new(:metrics => [:first_metric, :second_metric], :title => "My Widget", :type => :timeline)
    widget.metrics.length.should == 2
    widget.metrics.first.should be_a(FnordMetric::CountMetric)
    widget.metrics.first.token.should == :first_metric
    widget.metrics.last.should be_a(FnordMetric::CountMetric)
    widget.metrics.last.token.should == :second_metric
  end

  it "should define a new widget when given two metrics" do
    my_metrics = [
      FnordMetric.define(:first_metric, :count => :true),
      FnordMetric.define(:second_metric, :count => :true)
    ]
    widget = FnordMetric::Widget.new(:metrics => my_metrics, :title => "My Widget", :type => :timeline)
    widget.metrics.length.should == 2
    widget.metrics.first.should be_a(FnordMetric::CountMetric)
    widget.metrics.first.token.should == :first_metric
    widget.metrics.last.should be_a(FnordMetric::CountMetric)
    widget.metrics.last.token.should == :second_metric
  end

  it "should return the right ticks for 1h intervals with current" do
    t = Time.now
    widget = FnordMetric::Widget.new(:range => (t-2.days)..t, :tick => 1.hour)
    widget.ticks.length.should == 49
    ranges_should_match! widget.ticks.first, ((t-48.hours)..(t-47.hours))
    ranges_should_match! widget.ticks.last, (t..(t+1.hour))
  end

  it "should return the right ticks for 1h intervals without current" do
    t = Time.now
    widget = FnordMetric::Widget.new(:range => (t-2.days)..t, :tick => 1.hour, :include_current => false)
    widget.ticks.length.should == 48
    ranges_should_match! widget.ticks.first, ((t-48.hours)..(t-47.hours))
    ranges_should_match!widget.ticks.last, ((t-1.hour)..t)
  end

private
  
  def ranges_should_match!(a, b)
    (a.first.to_i - b.first.to_i).should == 0
    (a.last.to_i - b.last.to_i).should == 0
  end

end