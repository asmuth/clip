require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Namespace do

  before(:all) do
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)
  end

  before(:each) do
    @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
    @namespace = FnordMetric::Namespace.new(:myns_213, :redis_prefix => "fnordmetric")
  end

  it "should generate the correct redis prefix"

  describe "instance methods" do

    it "should create a new dashboard if a widget is added" do
      @namespace.widget("MyFooDash", nil)
      @namespace.dashboards.keys.should == ["MyFooDash"]
    end

    it "should create a new dashboard if the dashboard-title contains whitespaces" do
      @namespace.widget("My Dash", nil)
      @namespace.dashboards.keys.should == ["MyDash"]
    end

    it "should create a new dashboard if a widget is added and add the widget"
    it "should add widget to an existing dashboard"

  end

  describe "registering gauges" do

    it "should register a simple gauge" do
      @namespace.gauge(:fugauge, {:fnord => 23})
      @namespace.gauges[:fugauge].should be_a(FnordMetric::Gauge) 
    end

    it "should register a simple gauge and pass options" do
      @namespace.gauge(:fugauge2, {:fnord => 23})
      @namespace.gauges[:fugauge2].instance_variable_get(:@opts).should include({:fnord => 23})
      @namespace.gauges[:fugauge2].instance_variable_get(:@opts).should include({:key => :fugauge2})
    end

    it "should register a multi gauge" do
      pending "fixme"
      @namespace.numeric_gauge(:multigauge, {:fnord => 23, :ticks => [1.hour], :series => ["fnord"]})
      @namespace.gauges[:multigauge].should be_a(FnordMetric::NumericGauge) 
    end

    it "should register a multi gauge and pass options" do
      pending "fixme"
      @namespace.numeric_gauge(:multigauge2, {:fnord => 42, :ticks => [1.hour], :series => ["fnord"]})
      @namespace.gauges[:multigauge2].instance_variable_get(:@opts).should include({:fnord => 42})
      @namespace.gauges[:multigauge2].instance_variable_get(:@opts).should include({:key => :multigauge2})
    end

  end

  describe "registering event handlers" do

    it "should register an event handler" do
      @namespace.handlers.length.should == 0
      @namespace.event(:foobar){}
      @namespace.event(:fnordbar){}
      @namespace.handlers["foobar"].length.should == 1
      @namespace.handlers["fnordbar"].length.should == 1
      @namespace.handlers.length.should == 2
    end

    it "should register an event handler and create a context"
    it "should register an event handler and pass options"
    it "should register an event handler and pass gauges"
    it "should announce an event to the correct handler"
    it "should announce an event to multiple handlers"
    it "should announce an event to the wildcard handler"
  end

  it "should create a new session on announce if _session is set" do
    FnordMetric::Session.should_receive(:create).and_return(SessionMock.new)
    FnordMetric::Namespace.new(
      :myns_213, 
      :redis_prefix => "fnordmetric"
    ).ready!(@redis_wrap, @redis).announce(
      :_time => Time.now.to_i,
      :_type => "foobar", 
      :_session => "sess213"
    )
  end

  it "should add the event to the namespace-event-type-list" do
    FnordMetric::Namespace.new(
      :myns_215, 
      :redis_prefix => "fnordmetric"
    ).ready!(@redis_wrap, @redis).announce(
      :_eid => "35r2423",
      :_time => Time.now.to_i,
      :_type => "fnordbar",
      :_session => "sess213"
    )
    event_ids = @redis.lrange("fnordmetric-myns_215-type-fnordbar", 0, -1)
    event_ids.length.should == 1
    event_ids.first.should == "35r2423"
  end

  class SessionMock
    def session_key
      "asdasd"
    end
  end

end
