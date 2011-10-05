require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric do

  it "should build the car report" do
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

	report = FnordMetric.report(:range => (3.days.ago..Time.now))
	report.colors_total.should     == 3
	report.cars_total.should       == 7
	report.average_speed.should    == 113.6
	report.passengers_total.should == 26
	report.colors_total.should     == 3
  end
 
end