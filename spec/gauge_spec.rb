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
    }.should raise_error(key_error_klass)
  end

  it "should raise an error when initialize without key_prefix" do
    lambda{ 
      FnordMetric::Gauge.new({:key => "foo"})
    }.should raise_error(key_error_klass)
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

    it "should return the correct tick if configured" do
      gauge = FnordMetric::Gauge.new({:tick => 23, :key_prefix => "fnordmetrics-myns", :key => "mygauge"})
      gauge.tick.should == 23
    end

    it "should return the default tick if none configured" do
      gauge = FnordMetric::Gauge.new({:key_prefix => "fnordmetrics-myns", :key => "mygauge"})
      gauge.tick.should == 3600
    end

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
      @gauge.value_at(@now){ |v, t| v.to_i + 123 }.should == 177
    end

    it "should return the correct value_at per session" do
      @redis.hset(@gauge_key, "695280200", "76")
      @redis.set(@gauge_key+"-695280200-sessions-count", "23")
      _gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :unique => true,
        :redis => @redis
      })
      _gauge.value_at(@now).should == "76"
    end

    it "should return the correct value_at per session with avg" do
      @redis.hset(@gauge_key, "695280200", "76")
      @redis.set(@gauge_key+"-695280200-sessions-count", "23")
      _gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :unique => true,
        :average => true,
        :redis => @redis
      })
      _gauge.value_at(@now).should == (76.0/23.0)
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
      @redis.hset(@gauge_key, "695280200", "76")
      @redis.hset(@gauge_key, "695280210", "56")
      _gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :unique => true,
        :redis => @redis
      })
      _gauge.values_at([@now, @now+8]).should == {
        695280200 => "76",
        695280210 => "56"
      }
    end

    it "should receive gauge values per session for multiple ticks with avg" do  
      @redis.set(@gauge_key+"-695280200-sessions-count", "23")
      @redis.set(@gauge_key+"-695280210-sessions-count", "8")
      _gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :key => "mygauge_966",
        :unique => true,
        :average => true,
        :redis => @redis
      })
      _gauge.values_at([@now, @now+8]).should == {
        695280200 => (54.0/23.0),
        695280210 => (123.0/8.0)
      }
    end

    it "should receive gauge values with custom calculation for multiple ticks" do  
      @gauge.values_at([@now, @now+8]){ |val, time|
        val.to_i + 30
      }.should == {
        695280200 => 84,
        695280210 => 153
      }
    end

    it "should receive gauge values for all ticks in a given range" do
      @gauge.values_in(@now+10..@now+18).should == {
        695280200 => "54",
        695280210 => "123"
      }
    end

  end
  
  describe "three-dim value retrival" do

    before(:each) do
      @gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :three_dimensional => true,
        :key => "mygauge_966",
        :redis => @redis
      })
      @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
      @gauge_key = "fnordmetric-myns-gauge-mygauge_966-10-1323691200"
      @redis.zadd(@gauge_key, 18, "fnordyblubb")  
      @redis.zadd(@gauge_key, 23, "uberfoo")  
      @redis.set(@gauge_key+"-count", 41)
    end

    it "should retrieve field_values at a given time" do
      @gauge.field_values_at(1323691200).should be_a(Array)
      @gauge.field_values_at(1323691200).length.should == 2
      @gauge.field_values_at(1323691200)[0].should == ["uberfoo", "23"]
      @gauge.field_values_at(1323691200)[1].should == ["fnordyblubb", "18"]
    end

    it "should retrieve the correct total count" do
      @gauge.field_values_total(1323691200).should == 41
    end

    it "should retrieve max 50 fields per default" do
      70.times{ |n| @redis.zadd(@gauge_key, 23, "field#{n}") }
      @gauge.field_values_at(1323691200).should be_a(Array)
      @gauge.field_values_at(1323691200).length.should == 50
    end

    it "should retrieve more than 50 fields if requested" do
      70.times{ |n| @redis.zadd(@gauge_key, 23, "field#{n}") }
      @gauge.field_values_at(1323691200, :max_fields => 60).should be_a(Array)
      @gauge.field_values_at(1323691200, :max_fields => 60).length.should == 60
    end

    it "should retrieve all fields if requested" do
      70.times{ |n| @redis.zadd(@gauge_key, 23, "field#{n}") }
      @gauge.field_values_at(1323691200, :max_fields => 0).should be_a(Array)
      @gauge.field_values_at(1323691200, :max_fields => 0).length.should == 72
    end

    it "should call the value calculation block and return the result" do
      vals = @gauge.field_values_at(1323691200){ |v, t| v.to_i + 123 }
      vals.should be_a(Array)
      vals.length.should == 2
      vals[0].should == ["uberfoo", 146]
      vals[1].should == ["fnordyblubb", 141]
    end

    it "should return the correct field_values per session with avg" do
      @redis.set(@gauge_key+"-sessions-count", "3")
      @gauge = FnordMetric::Gauge.new({
        :tick => 10, 
        :key_prefix => "fnordmetric-myns", 
        :three_dimensional => true,
        :unique => true,
        :average => true,
        :key => "mygauge_966",
        :redis => @redis
      })
      vals = @gauge.field_values_at(1323691200)
      vals.should be_a(Array)
      vals.length.should == 2
      vals[0].should == ["uberfoo", 23/3.0]
      vals[1].should == ["fnordyblubb", 18/3.0]
    end

  end
end