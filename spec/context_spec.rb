require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Context do

  it "should fetch a gauge by key"
  it "should fetch a gauge by gauge"
  it "should raise an error if unable to fetch a gauge"

  it "should return the correct gauge key"

  it "should instance-eval the handler on call" do 
    Thread.current["fnord_123"] = false
    my_block = proc{ Thread.current["fnord_123"] = "35r25" }   
    context = FnordMetric::Context.new({}, my_block)
    context.call({}, nil)
    Thread.current["fnord_123"].should == "35r25"
  end

end