share_examples_for FnordMetric::GaugeLike do

  it "should remember its own name" do
    gauge = @gauge_klass.new({:key_prefix => "foo", :key => "fnordgauge", :ticks => [1.hour], :series => [:fu]}) 
    gauge.name.should == "fnordgauge"
  end

  it "should return its key as title if none specified" do
    gauge = @gauge_klass.new({:key_prefix => "foo", :key => "fnordgauge", :ticks => [1.hour], :series => [:fu]}) 
    gauge.title.should == "fnordgauge"
  end

  it "should return its title as title if none specified" do
    gauge = @gauge_klass.new({:key_prefix => "foo", :key => "fnordgauge", :ticks => [1.hour], :title => "Fnord Gauge", :series => [:fu]}) 
    gauge.title.should == "Fnord Gauge"
  end

  it "should raise an error when initialize without key" do
    lambda{ 
      @gauge_klass.new({:key_prefix => "foo", :ticks => [1.hour], :series => [:fu]})
    }.should raise_error(key_error_klass)
  end

  it "should raise an error when initialize without key_prefix" do
    lambda{ 
      @gauge_klass.new({:key => "foo", :ticks => [1.hour], :series => [:fu]})
    }.should raise_error(key_error_klass)
  end

=begin
  it "should generate the correct key without append" do
    gauge = @gauge_klass.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23, :ticks => [1.hour], :series => [ :fnord ]})
    if [@gauge_klass, @gauge_klass.superclass].include?(FnordMetric::MultiGauge)
      gauge.key.should == "fnordmetrics-myns-multigauge-mygauge"
    else
      gauge.key.should == "fnordmetrics-myns-gauge-mygauge-23"
    end
  end

  it "should generate the correct key with append" do
    gauge = @gauge_klass.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23, :ticks => [1.hour], :series => [ :fnord ]})
    if [@gauge_klass, @gauge_klass.superclass].include?(FnordMetric::MultiGauge)
      gauge.key(:fnord).should == "fnordmetrics-myns-multigauge-mygauge-fnord"
    else
      gauge.key(:fnord).should == "fnordmetrics-myns-gauge-mygauge-23-fnord"
    end
  end

  it "should add redis" do
    gauge = @gauge_klass.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23, :ticks => [1.hour], :series => [ :fnord ]})
    gauge.add_redis("FNORD")
    gauge.instance_variable_get(:"@opts")[:redis].should == "FNORD"
  end
=end

end
