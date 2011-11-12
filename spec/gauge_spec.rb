require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Gauge do

  it "should raise an error when initialize without key" do
  	lambda{ 
	  FnordMetric::Gauge.new({:key_prefix => "foo"}) 
	}.should raise_error(KeyError)
  end

  it "should raise an error when initialize without key_prefix" do
  	lambda{ 
	  FnordMetric::Gauge.new({:key => "foo"}) 
	}.should raise_error(KeyError)
  end

  it "should generate the correct key without append" do
  	gauge = FnordMetric::Gauge.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge"})
  	gauge.key.should == "fnordmetrics-myns-gauge-mygauge"
  end

  it "should generate the correct key with append" do
  	gauge = FnordMetric::Gauge.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge"})
  	gauge.key(:fnord).should == "fnordmetrics-myns-gauge-mygauge-fnord"
  end

  it "should return the correct tick if configured"
  it "should return the default tick if none configured"

  it "should return the correct tick_at"
  
end