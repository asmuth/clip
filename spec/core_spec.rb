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
  
end