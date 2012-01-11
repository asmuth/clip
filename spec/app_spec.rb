require ::File.expand_path('../spec_helper.rb', __FILE__)

include Rack::Test::Methods
include FnordMetric

describe "app" do

  before(:all) do
    redis = Redis.new
    @redis = RedisWrap.new(redis, false)
    @opts = { 
      :redis_prefix => "fnordmetric",
      :session_data_ttl => 120,
    }
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @namespace = Namespace.new(:foospace, @opts)
    @redis_wrap = RedisWrap.new(@redis)
  end

  def app
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil

        gauge :testgauge, :tick => 1.hour.to_i, :progressive => true
        gauge :test3gauge, :tick => 1.hour.to_i, :three_dimensional => true

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
      sess["_updated_at"].should == (@now-5).to_s
    end

    it "should render a list of all active sessions with usernames" do
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now, 
        :_type => "_set_name", 
        :_session => "sess213",
        :name => "Hans Peter"
      )
      get "/foospace/sessions" 
      sess = JSON.parse(last_response.body)["sessions"].first
      sess["_name"].should == "Hans Peter"
    end

    it "should render a list of all active sessions with user pictures" do
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now, 
        :_type => "_set_picture", 
        :_session => "sess213",
        :url => "http://myhost.com/mypic.jpg"
      )
      get "/foospace/sessions" 
      sess = JSON.parse(last_response.body)["sessions"].first
      sess["_picture"].should == "http://myhost.com/mypic.jpg"
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

  describe "events api: rendering events" do

    before(:each) do
      @redis.keys("fnordmetric-foospace*").each { |k| @redis.del(k) }  
    end

    it "should render a list of all events" do
      @namespace.ready!(@redis_wrap).announce(
        :_eid => "sdkjgh9sd8f",
        :_time => Time.now.to_i, 
        :_type => "foobar!!!"
      )
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 1
    end

    it "should render a list of all events including event-times" do
      @namespace.ready!(@redis_wrap).announce(
        :_eid => "sdkjgh9sd8f",
        :_time => @now-23, 
        :_type => "foobar!!!"
      )
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 1
      JSON.parse(last_response.body)["events"].first["_time"].to_i.should == @now-23
    end

    it "should render a list of all events including event-ids" do
      @namespace.ready!(@redis_wrap).announce(
        :_eid => "sdkjgh9sd8f",
        :_time => Time.now.to_i, 
        :_type => "foobar!!!"
      )
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 1
      JSON.parse(last_response.body)["events"].first["_eid"].should == "sdkjgh9sd8f"
    end


    it "should render a list of all events including event-types" do
      create_event("sdkjgh9sd8f", { 
        :_eid => "sdkjgh9sd8f",
        :_time => Time.now.to_i, 
        :_type => "foobar!!!"
      })
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 1
      JSON.parse(last_response.body)["events"].first["_type"].should == "foobar!!!"
    end

    it "should render a list of all events including _session-data" do
      create_event("65785678634", {
        :_eid => "65785678634",
        :_time => @now-23, 
        :_type => "foobar!!!",
        :_session => "blubb123"
      })
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 1
      JSON.parse(last_response.body)["events"].first["_session"].should == "blubb123"
    end

    it "should render a list of all events including _session-data" do
      create_event("4234234634", { 
        :_eid => "4234234634",
        :_time => @now-23, 
        :_type => "foobar!!!",
        :_session => "blubb123",
        :fnord => "yeah"
      })
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 1
      blubb123_md5 = "e5fa475b26873af5ec6c77668f9975a5"
      JSON.parse(last_response.body)["events"].first["_session_key"].should == blubb123_md5
    end

    it "should render a list of all events in the correct chronological order" do
      create_event("daasdasd", { 
        :_eid => "daasdasd",
        :_time => @now-23, 
        :_type => "foobar!!!"
      })
      create_event("345345345", { 
        :_eid => "345345345",
        :_time => @now-5, 
        :_type => "foobar!!!"
      })
      create_event("sdygsygsdg", { 
        :_eid => "sdygsygsdg",
        :_time => @now-16, 
        :_type => "foobar!!!"
      })
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 3
      JSON.parse(last_response.body)["events"][0]["_eid"].should == "345345345"
      JSON.parse(last_response.body)["events"][1]["_eid"].should == "sdygsygsdg"
      JSON.parse(last_response.body)["events"][2]["_eid"].should == "daasdasd"
    end

    it "should not render more than 100 events at a time" do
      120.times do |n|
        create_event("sdygsygsdg-#{n}", { 
          :_eid => "sdygsygsdg-#{n}",
          :_time => @now-16, 
          :_type => "foobar!!!"
        })
      end
      get "/foospace/events" 
      JSON.parse(last_response.body).should have_key("events")
      JSON.parse(last_response.body)["events"].length.should == 100
    end

    it "should render all events since a time, not including events at that exact time" do
      create_event("daasdasd", { 
        :_eid => "daasdasd",
        :_time => @now-23, 
        :_type => "foobar!!!"
      })
      create_event("345345345", { 
        :_eid => "345345345",
        :_time => @now-5, 
        :_type => "foobar!!!"
      })
      create_event("sdygsygsdg", { 
        :_eid => "sdygsygsdg",
        :_time => @now-16, 
        :_type => "foobar!!!"
      })
      get "/foospace/events?since=#{@now-24}" 
      JSON.parse(last_response.body)["events"].length.should == 3
      get "/foospace/events?since=#{@now-17}" 
      JSON.parse(last_response.body)["events"].length.should == 2
      get "/foospace/events?since=#{@now-16}" 
      JSON.parse(last_response.body)["events"].length.should == 1
      get "/foospace/events?since=#{@now-2}" 
      JSON.parse(last_response.body)["events"].length.should == 0
    end

    it "should render all events for a single event type" do
      @namespace.ready!(@redis_wrap).announce(
        :_type => "fn0rd",
        :_time => @now,
        :_eid => "124234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_type => "f00bar",
        :_time => @now,
        :_eid => "12235234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_type => "fn0rd",
        :_time => @now,
        :_eid => "124234234"
      )
      get "/foospace/events?type=fn0rd" 
      JSON.parse(last_response.body)["events"].length.should == 2
      get "/foospace/events?type=f00bar" 
      JSON.parse(last_response.body)["events"].length.should == 1
    end

    it "should render all events for a single session key" do
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now,
        :_session => "max_session",
        :_eid => "124234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now,
        :_session => "kate_session",
        :_eid => "12235234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now,
        :_session => "kate_session",
        :_eid => "124234234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_time => @now,
        :_eid => "124234234"
      )
      max_session_key = Digest::MD5.hexdigest "max_session"
      kate_session_key = Digest::MD5.hexdigest "kate_session"

      get "/foospace/events?session_key=#{max_session_key}"
      events = JSON.parse(last_response.body)["events"]
      events.length.should == 1

      get "/foospace/events?session_key=#{kate_session_key}"
      events = JSON.parse(last_response.body)["events"]
      events.length.should == 2
    end

    it "should render a list of event types" do
      @namespace.ready!(@redis_wrap).announce(
        :_type => "fn0rd",
        :_time => @now,
        :_eid => "124234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_type => "f00bar",
        :_time => @now,
        :_eid => "12235234"
      )
      @namespace.ready!(@redis_wrap).announce(
        :_type => "fn0rd",
        :_time => @now,
        :_eid => "124234234"
      )
      get "/foospace/event_types" 
      JSON.parse(last_response.body)["types"].length.should == 2
      JSON.parse(last_response.body)["types"].should include("fn0rd")
      JSON.parse(last_response.body)["types"].should include("f00bar")
    end

    it "should render all events for a single session"

    it "should render all events for a single session since a unix timestamp"

    it "should render all events for a single session, but not more than 100"

    def create_event(event_id, event_data)        
      @redis_wrap.zadd(@namespace.key_prefix(:timeline), event_data.delete(:_time), event_id)
      @redis_wrap.set("fnordmetric-event-#{event_id}", event_data.to_json)        
    end

  end

  describe "events api: creating events" do

    it "should track an event without auth" do      
      post "/events", :_type => "myevent", :fnord => "foobar"
      last_response.status.should == 200
    end

    it "should return 400 if no type is provided" do
      pending("fix this")
    	post "/events", :fnord => "foobar"
    	last_response.status.should == 400  	
    	last_response.body.should == "please specify the event_type"
    end

  end

  describe "gauges api" do

    before(:all) do
      @redis.keys("fnordmetric-foospace*").each { |k| @redis.del(k) }  
      gauge_key = "fnordmetric-foospace-gauge-testgauge-#{1.hour.to_i}"
      @redis.hset(gauge_key, 1323687600, "18")  
      @redis.hset(gauge_key, 1323691200, "23")
    end

    it "should return the right answer for: /metric/:name?at=timestamp" do      
      get "/foospace/gauge/testgauge?at=1323691205"
      JSON.parse(last_response.body).first.last.to_i.should == 23
    end

    it "should return the right answer for: /metric/:name?at=timestamp" do      
      get "/foospace/gauge/testgauge?at=1323691200"
      JSON.parse(last_response.body).first.last.to_i.should == 23
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp" do  
      get "/foospace/gauge/testgauge?at=1323691200-1323691205"
      JSON.parse(last_response.body).first.last.to_i.should == 18
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp" do  
      get "/foospace/gauge/testgauge?at=1323691201-1323695205"
      JSON.parse(last_response.body).keys.length
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp" do  
      get "/foospace/gauge/testgauge?at=1323691199-1323701201"
      JSON.parse(last_response.body).keys.length.should == 4
      JSON.parse(last_response.body)["1323687600"].to_i.should == 18
      JSON.parse(last_response.body)["1323691200"].to_i.should == 23
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&sum=true" do  
      get "/foospace/gauge/testgauge?at=1323691199-1323701201&sum=true"
      JSON.parse(last_response.body).keys.length.should == 1
      JSON.parse(last_response.body)["sum"].to_i.should == 18+23
    end

  end

  describe "three-dim gauges api" do

    before(:all) do
      @redis.keys("fnordmetric-foospace*").each { |k| @redis.del(k) }  
      gauge_key = "fnordmetric-foospace-gauge-test3gauge-#{1.hour.to_i}-1323691200"
      @redis.zadd(gauge_key, 18, "fnordyblubb")  
      @redis.zadd(gauge_key, 23, "uberfoo")  
      @redis.set(gauge_key+"-count", 41)
    end

    it "should return the right answer for: /metric/:name?at=timestamp" do      
      get "/foospace/gauge/test3gauge?at=1323691205"
      JSON.parse(last_response.body)["count"].to_i.should == 41
      JSON.parse(last_response.body)["values"].length.should == 2
      JSON.parse(last_response.body)["values"][0].should == ["uberfoo", "23"]
      JSON.parse(last_response.body)["values"][1].should == ["fnordyblubb", "18"]
    end

  end

end
