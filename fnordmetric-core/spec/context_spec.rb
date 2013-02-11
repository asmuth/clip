require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Context do

  before(:all) do
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)
  end

  before(:each) do
    @redis.keys("fnordmetrictest*").each { |k| @redis.del(k) }  
  end

  it "should fetch a gauge by key"
  it "should fetch a gauge by gauge"
  it "should raise an error if unable to fetch a gauge"

  it "should expose the current session_key" do
    ns = FnordMetric::Namespace.new(
      :myns_253,
      :redis_prefix => "fnordmetrictest"
    )
    ns.opt_event(:fnordbar, {}) do
      session_key.should == "145d3f6dee1a770e3a3c550b3993dc81"
    end
    ns.ready!(@redis_wrap, @redis).announce(
      :_eid => "35r2423",
      :_time => Time.now.to_i, 
      :_type => "fnordbar", 
      :_session => "sess213"
    )
  end

  it "should instance-eval the handler on call" do 
    Thread.current["fnord_123"] = false
    my_block = proc{ Thread.current["fnord_123"] = "35r25" }   
    context = FnordMetric::Context.new({}, my_block)
    context.call({}, nil, nil)
    Thread.current["fnord_123"].should == "35r25"
  end

end
