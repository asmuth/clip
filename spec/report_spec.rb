require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Report do

  describe "Car Report" do

    before(:each) do
      FnordMetric.reset_metrics
      FnordMetric::Event.destroy_all
      build_car_report_for_test!
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

  end

  describe "Metric Types" do

    before(:each) do
      FnordMetric.reset_metrics
      FnordMetric::Event.destroy_all
    end
  
    it "should create a sum-metric if the sum-option is provided" do
      FnordMetric.metric(:testmetric, :sum => :field_name)
      report = FnordMetric.report(:range => (3.days.ago..Time.now))
      report.testmetric.should be_a(FnordMetric::SumMetric)
    end

    it "should create avg-metric if the avg-option is provided" do
      FnordMetric.metric(:testmetric, :average => :field_name)
      report = FnordMetric.report(:range => (3.days.ago..Time.now))
      report.testmetric.should be_a(FnordMetric::AverageMetric)
    end

    it "should create a count-metric if the count-option is provided" do
      FnordMetric.metric(:testmetric, :count => true)
      report = FnordMetric.report(:range => (3.days.ago..Time.now))
      report.testmetric.should be_a(FnordMetric::CountMetric)
    end

    it "should create a sum-metric if the sum-option is provided" do
      FnordMetric.metric(:testmetric, :combine => :lambda)
      report = FnordMetric.report(:range => (3.days.ago..Time.now))
      report.testmetric.should be_a(FnordMetric::CombineMetric)
    end

  end

  private

  def build_car_report_for_test!
    FnordMetric.metric(:colors_total, :count => true, :unique => :color) 
    FnordMetric.metric(:cars_total, :count => true) 
    FnordMetric.metric(:passengers_total, :sum => :passengers) 
    FnordMetric.metric(:average_speed, :average => :speed)
    FnordMetric.track('car_seen', :color => "red",  :speed => 130, :passengers => 2)
    FnordMetric.track('car_seen', :color => "pink", :speed => 150, :passengers => 1)
    FnordMetric.track('car_seen', :color => "red",  :speed => 65,  :passengers => 4)
    FnordMetric.track('car_seen', :color => "blue", :speed => 100, :passengers => 2)
    FnordMetric.track('car_seen', :color => "red",  :speed => 123, :passengers => 2)
    FnordMetric.track('car_seen', :color => "blue", :speed => 130, :passengers => 3)
    FnordMetric.track('car_seen', :color => "red",  :speed => 142, :passengers => 2)
  end

end