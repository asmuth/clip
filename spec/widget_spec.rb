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

  it "should raise an error if anything is rendered before the report is added" do
  	FnordMetric.define(:my_metric, :sum => :my_field)
  	report = FnordMetric.report(:range => (4.days.ago..Time.now))
  	widget = FnordMetric::Widget.new
    lambda{ widget.metrics }.should raise_error(RuntimeError)
    widget.add_report(FnordMetric.report(:range => (4.days.ago..Time.now)))
    lambda{ widget.metrics }.should_not raise_error(RuntimeError)
  end

end