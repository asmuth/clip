require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Report do

  before(:each) do
    FnordMetric.reset_metrics
    FnordMetric::Event.destroy_all
    build_report_for_test!
  end

  it "should build the car report" do
    report = FnordMetric.report(:range => (3.days.ago..Time.now))
    report.should be_a(FnordMetric::Report)
    report.metrics.length.should == 4
  end

  it "should return a metrics object for each defined metric" do
    report = FnordMetric.report(:range => (3.days.ago..Time.now))
    report.colors_total.should be_a(FnordMetric::Metric)
    report.cars_total.should be_a(FnordMetric::Metric)
    report.average_speed.should be_a(FnordMetric::Metric)
    report.passengers_total.should be_a(FnordMetric::Metric)
  end

  it "should have the right total/current values" do
    report = FnordMetric.report(:range => (3.days.ago..Time.now))
    report.colors_total.current.should     == 3
    report.cars_total.current.should       == 7
    report.average_speed.current.should    == 113.6
    report.passengers_total.current.should == 26
  end
 
 private

  def build_report_for_test!
    FnordMetric.define(:colors_total, :count => true, :unique => :color) 
    FnordMetric.define(:cars_total, :count => true) 
    FnordMetric.define(:passengers_total, :sum => :passengers) 
    FnordMetric.define(:average_speed, :average => :speed)
    FnordMetric.track('car_seen', :color => "red",  :speed => 130, :passengers => 2)
    FnordMetric.track('car_seen', :color => "pink", :speed => 150, :passengers => 1)
    FnordMetric.track('car_seen', :color => "red",  :speed => 65,  :passengers => 4)
    FnordMetric.track('car_seen', :color => "blue", :speed => 100, :passengers => 2)
    FnordMetric.track('car_seen', :color => "red",  :speed => 123, :passengers => 2)
    FnordMetric.track('car_seen', :color => "blue", :speed => 130, :passengers => 3)
    FnordMetric.track('car_seen', :color => "red",  :speed => 142, :passengers => 2)
  end

end