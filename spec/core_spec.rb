require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  it "should define a new metric" do
    FnordMetric.define(:myfield_total, :sum => :myfield)
    FnordMetric.metrics.keys.should include(:myfield_total)
    FnordMetric.metrics[:myfield_total].should be_a(FnordMetric::Metric)
  end

  it "should define a new dashboard" do
    FnordMetric.dashboard('My Dashboard'){ |dash| }
    dashboard = FnordMetric.dashboards.last
    dashboard.should be_a(FnordMetric::Dashboard)
    dashboard.title.should == 'My Dashboard'
  end

  it "should define a new dashboard and call the config block" do
    block_called = false
    FnordMetric.dashboard 'My Dashboard' do |dash|
      block_called = true
      dash.should be_a(FnordMetric::Dashboard)
    end
    block_called.should be_true
  end

  it "should define a new widget" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget", :type => :timeline)
    FnordMetric.widgets[:my_widget].title.should == "My Widget"
  end

  it "should raise an error if no type option is provided" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    lambda{
      FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget")      
    }.should raise_error(RuntimeError)
  end

  it "should raise an error if an unknown metric is added to a widget" do
    lambda{
      FnordMetric.widget(:my_widget, :metrics => :my_unknown_metric, :title => "My Widget", :type => :timeline)      
    }.should raise_error(RuntimeError)
  end

end