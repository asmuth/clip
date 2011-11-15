require ::File.expand_path('../spec_helper.rb', __FILE__)

include Rack::Test::Methods
include FnordMetric

describe "app" do

  before(:all) do
    @redis = Redis.new
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @namespace = Namespace.new(:foospace, :redis_prefix => "fnordmetric")
    @redis_wrap = RedisWrap.new(@redis)
    @opts = { :redis_prefix => "fnordmetric" }
  end

  def app
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil
      }
    }, @opts)
  end

  it "should redirect to the first namespace" do
  	get "/"
  	last_response.status.should == 302
  	last_response.location.should == "http://example.org/foospace"
  end

  it "should render the namespace" do
    get "/foospace"
    last_response.status.should == 200
  end

  it "should render the dashboards" do
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil
      }
    }, @opts)
  	get "/foospace"
  	last_response.status.should == 200
    last_response.body.should include("Blubb")
  end

  it "should render the dashboards" do
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil
        widget 'Fnord', nil
      }
    }, @opts)
    get "/foospace"
    last_response.status.should == 200
    last_response.body.should include("Blubb")
    last_response.body.should include("Fnord")
  end

  describe "sessions api" do
  
    before(:each) do
      @redis.keys("fnordmetric-foospace*").each { |k| @redis.del(k) }  
    end

    it "should render a list of all active sessions" do
      @namespace.ready!(@redis_wrap).announce(
        :_time => Time.now.to_i, 
        :_type => "foobar", 
        :_session => "sess213"
      )
      get "/foospace/sessions" 
      JSON.parse(last_response.body).should have_key("sessions")
      JSON.parse(last_response.body)["sessions"].length.should == 1
    end

    it "should render a list of all active sessions with timestamps" do
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now-5, 
        :_type => "foobar", 
        :_session => "sess213"
      )
      get "/foospace/sessions" 
      sess = JSON.parse(last_response.body)["sessions"].first
      sess["last_updated"].should == @now-5
    end

    it "should render a list of all active sessions with hashed keys" do
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now-5, 
        :_type => "foobar", 
        :_session => "sess133"
      )
      get "/foospace/sessions" 
      sess = JSON.parse(last_response.body)["sessions"].first
      sess["session_key"].should == Digest::MD5.hexdigest("sess133")
    end

    it "should not render more than 100 sessions at once" do
      123.times do |n|
        @namespace.ready!(@redis_wrap).announce(
          :_time => Time.now.to_i, 
          :_type => "foobar", 
          :_session => "sess213-#{n}"
        )
      end
      get "/foospace/sessions" 
      JSON.parse(last_response.body).should have_key("sessions")
      JSON.parse(last_response.body)["sessions"].length.should == 100
    end

    it "should render sessions ordered by last_updated_at" do
      context = @namespace.ready!(@redis_wrap)
      context.announce(
        :_time => Time.now.to_i, 
        :_type => "foobar", 
        :_session => "sessfoo"
      )
      context.announce(
        :_time => Time.now.to_i-23, 
        :_type => "foobar", 
        :_session => "sessbar"
      )
      context.announce(
        :_time => Time.now.to_i-5, 
        :_type => "foobar", 
        :_session => "sessfnord"
      )
      get "/foospace/sessions" 
      sessions = JSON.parse(last_response.body)["sessions"]
      sessions.length.should == 3
      sessions[0]["session_key"].should == Digest::MD5.hexdigest("sessfoo")
      sessions[1]["session_key"].should == Digest::MD5.hexdigest("sessfnord")
      sessions[2]["session_key"].should == Digest::MD5.hexdigest("sessbar")
    end

  end

  describe "events api" do

    it "should track an event without auth" do
      pending("fix this")
      #post "/events", :type => "myevent", :fnord => "foobar"
      #last_response.status.should == 200
      #FnordMetric::Event.last.type.should == "myevent"
      #FnordMetric::Event.last.fnord.should == "foobar"
    end

    it "should return 400 if no type is provided" do
      pending("fix this")
    	#post "/events", :fnord => "foobar"
    	#last_response.status.should == 400  	
    	#last_response.body.should == "please specify the event_type"
    end

    it "should track an event in the past" do
      pending("fix this")
    	#my_time = (Time.now-3.years).to_i
    	#post "/events", :type => "myevent", :time => my_time
    	#last_response.status.should == 200
    	#FnordMetric::Event.last.type.should == "myevent"
    	#FnordMetric::Event.last.time.should == my_time
    end

    it "should track an event with integer data" do
      pending("fix this")
    	#post "/events", :type => "myevent", :blubb => "123"
    	#last_response.status.should == 200
    	#FnordMetric::Event.last.type.should == "myevent"
    	#FnordMetric::Event.last.blubb.should == 123
    end
    
    it "should track an event with float data" do
      pending("fix this")
    	#post "/events", :type => "myevent", :blubb => "42.23"
    	#last_response.status.should == 200
    	#FnordMetric::Event.last.type.should == "myevent"
    	#FnordMetric::Event.last.blubb.should == 42.23
    end

  end

  describe "metrics api" do
    # copy from _spec/app_spec.rb ?
  end


end