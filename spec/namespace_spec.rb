require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Namespace do


  before(:all) do
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)
  end

  before(:each) do
    @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
  end

  describe "instance methods" do

    before(:each) do
      @namespace = FnordMetric::Namespace.new(:myns_213, :redis_prefix => "fnordmetric")
    end

    it "should create a new dashboard if a widget is added" do
      @namespace.widget("My Dash", nil)
      @namespace.dashboards.keys.should == ["My Dash"]
    end

    it "should create a new dashboard if a widget is added and add the widget"
    it "should add widget to an existing dashboard"

  end


  it "should generate the correct redis prefix"

  it "should register a gauge"
  it "should register a gauge and pass options"

  describe "registering event handlers" do

    before(:each) do
      @namespace = FnordMetric::Namespace.new(:myns_213, :redis_prefix => "fnordmetric")
    end

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

    it "should announce an event to the correct handler" do
      pending("finish this")
    # block_called = false
    # FnordMetric::Dashboard.new(:title => 'My Dashboard') do |dash|
    #    block_called = true
    #    dash.should be_a(FnordMetric::Dashboard)
    #  end
    #  block_called.should be_true
    end

    it "should announce an event to multiple handlers"
    it "should announce an event to the wildcard handler"

  end

  it "should create a new session on announce if _session is set" do
    FnordMetric::Session.should_receive(:create).and_return(SessionMock.new)
    FnordMetric::Namespace.new(
      :myns_213, 
      :redis_prefix => "fnordmetric"      
    ).ready!(@redis_wrap).announce(
      :_time => Time.now.to_i, 
      :_type => "foobar", 
      :_session => "sess213"
    )
  end

  it "should add the event to the namespace-event-type-list" do
    FnordMetric::Namespace.new(
      :myns_215, 
      :redis_prefix => "fnordmetric"      
    ).ready!(@redis_wrap).announce(
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