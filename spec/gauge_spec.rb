require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Gauge do

  before(:all) do    
    @now = Time.utc(1992,01,13,5,23,23).to_i    
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis, false)
  end

  before(:each) do
    @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
  end

  it "should remember its own name" do
    gauge = FnordMetric::Gauge.new({:key_prefix => "foo", :key => "fnordgauge"}) 
    gauge.name.should == "fnordgauge"
  end

  it "should return its key as title if none specified" do
    gauge = FnordMetric::Gauge.new({:key_prefix => "foo", :key => "fnordgauge"}) 
    gauge.title.should == "fnordgauge"
  end

  it "should return its title as title if none specified" do
    gauge = FnordMetric::Gauge.new({:key_prefix => "foo", :key => "fnordgauge", :title => "Fnord Gauge"}) 
    gauge.title.should == "Fnord Gauge"
  end

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
  	gauge = FnordMetric::Gauge.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23})
  	gauge.key.should == "fnordmetrics-myns-gauge-mygauge-23"
  end

  it "should generate the correct key with append" do
  	gauge = FnordMetric::Gauge.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge", :tick => 23})
  	gauge.key(:fnord).should == "fnordmetrics-myns-gauge-mygauge-23-fnord"
  end

  describe "ticks" do

    it "should return the correct tick if configured"
    it "should return the default tick if none configured"


    it "should return the correct tick_at" do
      gauge = FnordMetric::Gauge.new({:tick => 10, :key_prefix => "fnordmetrics-myns", :key => "mygauge"})
      gauge.tick_at(@now).should == 695280200
      gauge.tick_at(@now+6).should == 695280200
      gauge.tick_at(@now+8).should == 695280210
    end

  end

  describe "value retrival" do

    before(:each) do
      @gauge_key = "fnordmetric-myns-gauge-mygauge_966-10"    
      @redis.hset(@gauge_key, "695280200", "54")
      @redis.hset(@gauge_key, "695280210", "123")
      @gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :redis => @redis
      })
    end

    it "should retrieve a gauge value at a given time" do
      @gauge.value_at(@now).should == "54"
      @gauge.value_at(@now+6).should == "54"
      @gauge.value_at(@now+8).should == "123"
    end

    it "should retrieve a gauge value at the current tick"

    it "should call the value calculation block and return the result" do
      @gauge.value_at(@now){ |v| v.to_i + 123 }.should == 177
    end

    it "should return the correct value_at per session" do
      @redis.set(@gauge_key+"-695280200-sessions-count", "23")
      _gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :unique => true,
        :redis => @redis
      })
      _gauge.value_at(@now).should == (54.0/23.0)
    end

    it "should receive gauge values for multiple ticks" do
      @gauge.values_at([@now, @now+8]).should == {
        695280200 => "54",
        695280210 => "123"
      }
    end

    it "should receive gauge values per session for multiple ticks" do  
      @redis.set(@gauge_key+"-695280200-sessions-count", "23")
      @redis.set(@gauge_key+"-695280210-sessions-count", "8")
      _gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :unique => true,
        :redis => @redis
      })
      _gauge.values_at([@now, @now+8]).should == {
        695280200 => (54.0/23.0),
        695280210 => (123.0/8.0)
      }
    end

    it "should receive gauge values with custom calculation for multiple ticks" do  
      @gauge.values_at([@now, @now+8]){ |val|
        val.to_i + 30
      }.should == {
        695280200 => 84,
        695280210 => 153
      }
    end

    it "should receive gauge values for all ticks in a given range" do
      @gauge.values_in(@now..@now+8).should == {
        695280200 => "54",
        695280210 => "123"
      }
      @gauge.values_in(@now..@now+6).should == {
        695280200 => "54"
      }
      @gauge.values_in(@now+8..@now+10).should == {
        695280210 => "123"
      }
    end

  end
  
end