require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::GaugeModifiers do

  before(:all) do    
    @now = Time.utc(1992,01,13,5,23,23).to_i    
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)
  end

  before(:each) do
    @redis.keys("fnordmetrics-myns*").each { |k| @redis.del(k) }  
  end

  describe "increment non-progressive gauges" do

    it "should increment a non-progressive gauge by 1" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_234-10"
      @redis.hset(gauge_key, "695280200", "12")  
      create_gauge_context({
        :key => "mygauge_234", 
        :tick => 10,
      }, proc{ 
        incr(:mygauge_234, 1)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "13"
    end

    it "should increment a non-progressive gauge by 5" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_224-10"
      @redis.hset(gauge_key, "695280200", "18")  
      create_gauge_context({
        :key => "mygauge_224", 
        :tick => 10,
      }, proc{ 
        incr(:mygauge_224, 5)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "23"
    end

    it "should increment a non-progressive gauge by event_value" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_253-10"
      @redis.hset(gauge_key, "695280200", "11")  
      create_gauge_context({
        :key => "mygauge_253", 
        :tick => 10,
      }, proc{ 
        incr(:mygauge_253, data[:myval].to_i)  
      }).tap do |context|      
        event = { :_time => @now, :myval => "25" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "36"
    end

  end
  

  describe "increment progressive gauges" do

    it "should increment a progressive gauge by 1" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_123-10"
      @redis.hset(gauge_key, "695280200", "23")  
      @redis.set(gauge_key+"-head",  "23")  
      create_gauge_context({
        :key => "mygauge_123", 
        :tick => 10,
        :progressive => true
      }, proc{ 
        incr(:mygauge_123, 1)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "24"
      @redis.get(gauge_key+"-head").should == "24"
    end

    it "should increment a progressive gauge by 5" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_125-10"
      @redis.hset(gauge_key, "695280200", "22")  
      @redis.set(gauge_key+"-head",  "22")  
      create_gauge_context({
        :key => "mygauge_125", 
        :tick => 10,
        :progressive => true
      }, proc{ 
        incr(:mygauge_125, 5)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "27"
      @redis.get(gauge_key+"-head").should == "27"
    end

    it "should increment a progressive gauge by 1 and copy head" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_128-10"    
      @redis.hdel(gauge_key, "695280200")
      @redis.set(gauge_key+"-head", "17")  
      create_gauge_context({
        :key => "mygauge_128", 
        :tick => 10,
        :progressive => true
      }, proc{ 
        incr(:mygauge_128, 3)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "20"
      @redis.get(gauge_key+"-head").should == "20"
    end

    it "should increment a progressive gauge by 5 and copy head" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_121-10"    
      @redis.hdel(gauge_key, "695280200")
      @redis.set(gauge_key+"-head", "19")  
      create_gauge_context({
        :key => "mygauge_121", 
        :tick => 10,
        :progressive => true
      }, proc{ 
        incr(:mygauge_121, 6)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "25"
      @redis.get(gauge_key+"-head").should == "25"
    end

    it "should raise an error if incr is called on a 3d gauge" do
      create_gauge_context({
        :key => "mygauge_167", 
        :tick => 10,
        :three_dimensional => true
      }, proc{ 
        incr(:mygauge_167, 1)  
      }).tap do |context|      
        event = { :_time => @now }
        lambda{
          context.call(event, @redis_wrap)
        }.should raise_error(RuntimeError)
      end
    end

  end

  describe "increment an average-gauge" do

    it "should increment_unique a non-progressive gauge" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_917-10"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-695280200-value-count", 5)
      create_gauge_context({
        :key => "mygauge_917", 
        :average => true,
        :tick => 10
      }, proc{ 
        incr(:mygauge_917, 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "84"
      @redis.get(gauge_key+"-695280200-value-count").should == "6"
    end

  end

  describe "increment uniquely by session_key" do

    it "should increment_unique a non-progressive gauge" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_963-10"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-695280200-sessions-count", 5)
      create_gauge_context({
        :key => "mygauge_963", 
        :unique => true,
        :tick => 10
      }, proc{ 
        incr(:mygauge_963, 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "84"
      @redis.get(gauge_key+"-695280200-sessions-count").should == "6"
      @redis.smembers(gauge_key+"-695280200-sessions").should == ["mysesskey"]
    end

    it "should not increment_unique a non-progressive gauge if session is known" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_966"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-695280200-sessions-count", 5)
      @redis.sadd(gauge_key+"-695280200-sessions", "mysesskey")
      create_gauge_context({
        :key => "mygauge_966", 
        :unique => true,
        :tick => 10
      }, proc{ 
        incr(:mygauge_966, 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "54"
      @redis.get(gauge_key+"-695280200-sessions-count").should == "5"
      @redis.smembers(gauge_key+"-695280200-sessions").should == ["mysesskey"]
    end

    it "should not increment_unique a non-progressive gauge w/o session" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_966"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-695280200-sessions-count", 5)
      create_gauge_context({
        :key => "mygauge_966", 
        :unique => true,
        :tick => 10
      }, proc{ 
        incr(:mygauge_966, 30)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "54"
      @redis.get(gauge_key+"-695280200-sessions-count").should == "5"
      @redis.smembers(gauge_key+"-695280200-sessions").should == []
    end


    it "should increment_unique a non-progressive gauge" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_963-10"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-progressive-sessions-count", 5)
      create_gauge_context({
        :key => "mygauge_963", 
        :tick => 10,
        :unique => true,
        :progressive => true
      }, proc{ 
        incr(:mygauge_963, 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "84"
      @redis.get(gauge_key+"-progressive-sessions-count").should == "6"
      @redis.smembers(gauge_key+"-progressive-sessions").should == ["mysesskey"]
    end

    it "should not increment_unique a non-progressive gauge if session is known" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_966"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-progressive-sessions-count", 5)
      @redis.sadd(gauge_key+"-progressive-sessions", "mysesskey")
      create_gauge_context({
        :key => "mygauge_966", 
        :tick => 10,
        :unique => true,
        :progressive => true
      }, proc{ 
        incr(:mygauge_966, 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "54"
      @redis.get(gauge_key+"-progressive-sessions-count").should == "5"
      @redis.smembers(gauge_key+"-progressive-sessions").should == ["mysesskey"]
    end

  end

describe "increment three-dimensional gagues" do

    it "should increment a three-dim gauge by 1" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_434-10-695280200"
      @redis.zadd(gauge_key, 12, "whoopwhoop")  
      create_gauge_context({
        :key => "mygauge_434", 
        :three_dimensional => true,
        :tick => 10,
      }, proc{ 
        incr_field(:mygauge_434, "whoopwhoop", 1)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.zscore(gauge_key, "whoopwhoop").to_f.should == 13.0
      @redis.get(gauge_key+"-count").should == "1"
    end

    it "should increment a three-dim gauge by 5 on an empty field" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_634-10-695280200"
      @redis.set(gauge_key+"-count", 6)
      create_gauge_context({
        :key => "mygauge_634", 
        :three_dimensional => true,
        :tick => 10,
      }, proc{ 
        incr_field(:mygauge_634, "whoopwhoop", 5)  
      }).tap do |context|      
        event = { :_time => @now }
        context.call(event, @redis_wrap)
      end
      @redis.zscore(gauge_key, "whoopwhoop").to_f.should == 5.0
      @redis.get(gauge_key+"-count").should == "7"
    end

    it "should increment a three-dim gauge by event supplied field" do
      gauge_key = "fnordmetrics-myns-gauge-mygauge_634-10-695280200"
      @redis.zadd(gauge_key, 11, "fnordybar")  
      create_gauge_context({
        :key => "mygauge_634", 
        :three_dimensional => true,
        :tick => 10,
      }, proc{ 
        incr_field(:mygauge_634, data[:myfield], 5)  
      }).tap do |context|      
        event = { :_time => @now, :myfield => "fnordybar" }
        context.call(event, @redis_wrap)
      end
      @redis.zscore(gauge_key, "fnordybar").to_f.should == 16.0
    end

    it "should increment_unique a three-dim gauge" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_1263-10-695280200"    
      @redis.zadd(gauge_key, 54, "mykey")
      @redis.set(gauge_key+"-sessions-count", 5)
      create_gauge_context({
        :key => "mygauge_1263", 
        :tick => 10,
        :unique => true,
        :three_dimensional => true
      }, proc{ 
        incr_field(:mygauge_1263, "mykey", 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.zscore(gauge_key, "mykey").to_f.should == 84.0
      @redis.get(gauge_key+"-sessions-count").should == "6"
      @redis.smembers(gauge_key+"-sessions").should == ["mysesskey"]
    end

    it "should not increment_unique a non-progressive gauge if session is known" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_1266-695280200"    
      @redis.zadd(gauge_key, 54, "otherkey")
      @redis.set(gauge_key+"-sessions-count", 5)
      @redis.sadd(gauge_key+"-sessions", "mysesskey")
      create_gauge_context({
        :key => "mygauge_1266", 
        :tick => 10,
        :unique => true,
        :three_dimensional => true
      }, proc{ 
        incr_field(:mygauge_1266, "otherkey", 30)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.zscore(gauge_key, "otherkey").to_f.should == 54.0
      @redis.get(gauge_key+"-sessions-count").should == "5"
      @redis.smembers(gauge_key+"-sessions").should == ["mysesskey"]
    end

  end

  describe "set value on two/three-dim gauge" do

    it "should set a value on a two-dim gauge" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_5463-10"    
      @redis.hset(gauge_key, "695280200", "54")
      @redis.set(gauge_key+"-695280200-sessions-count", 5)
      @redis.hget(gauge_key, "695280200").should == "54"
      create_gauge_context({
        :key => "mygauge_5463",
        :tick => 10
      }, proc{ 
        set_value(:mygauge_5463, 17)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.hget(gauge_key, "695280200").should == "17"
    end


    it "should set a value on a two-dim gauge" do  
      gauge_key = "fnordmetrics-myns-gauge-mygauge_1463-10-695280200"
      @redis.zadd(gauge_key, 65, "asdasdkey")
      @redis.zscore(gauge_key, "asdasdkey").to_f.should == 65.0
      create_gauge_context({
        :key => "mygauge_1463",
        :three_dimensional => true,
        :tick => 10
      }, proc{ 
        set_field(:mygauge_1463, "asdasdkey", 23)  
      }).tap do |context|      
        event = { :_time => @now, :_session_key => "mysesskey" }
        context.call(event, @redis_wrap)
      end
      @redis.zscore(gauge_key, "asdasdkey").to_f.should == 23.0
    end

  end


  it "should raise an error if incr_field is called on a 2d gauge"

private

  def create_gauge_context(opts, block)
    gauge = FnordMetric::Gauge.new({
      :key_prefix => "fnordmetrics-myns"
    }.merge(opts))      
    FnordMetric::Context.new({
      :gauges => { opts[:key].intern => gauge }
    }, block) 
  end


end