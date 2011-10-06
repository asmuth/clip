require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  it "should define a new metric" do
    FnordMetric.define(:myfield_total, :sum => :myfield)
    FnordMetric.metrics.keys.should include(:myfield_total)
    FnordMetric.metrics[:myfield_total][:name].should == :myfield_total
    FnordMetric.metrics[:myfield_total][:sum].should == :myfield
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

end