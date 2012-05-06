share_examples_for FnordMetric::GaugeLike do

  it "should remember its own name" do
    gauge = @gauge_klass.new({:key_prefix => "foo", :key => "fnordgauge"}) 
    gauge.name.should == "fnordgauge"
  end

  it "should return its key as title if none specified" do
    gauge = @gauge_klass.new({:key_prefix => "foo", :key => "fnordgauge"}) 
    gauge.title.should == "fnordgauge"
  end

  it "should return its title as title if none specified" do
    gauge = @gauge_klass.new({:key_prefix => "foo", :key => "fnordgauge", :title => "Fnord Gauge"}) 
    gauge.title.should == "Fnord Gauge"
  end

  it "should raise an error when initialize without key" do
    lambda{ 
      @gauge_klass.new({:key_prefix => "foo"})
    }.should raise_error(key_error_klass)
  end

  it "should raise an error when initialize without key_prefix" do
    lambda{ 
      @gauge_klass.new({:key => "foo"})
    }.should raise_error(key_error_klass)
  end

  it "should generate the correct key without append" do
    gauge = @gauge_klass.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23})
    gauge.key.should == "fnordmetrics-myns-gauge-mygauge-23"
  end

  it "should generate the correct key with append" do
    gauge = @gauge_klass.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23})
    gauge.key(:fnord).should == "fnordmetrics-myns-gauge-mygauge-23-fnord"
  end

  it "should add redis" do
    gauge = @gauge_klass.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23})
    gauge.add_redis("FNORD")
    gauge.instance_variable_get(:"@opts")[:redis].should == "FNORD"
  end

end