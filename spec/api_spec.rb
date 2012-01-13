require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Event do

  include FnordMetric

  before(:all) do
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)

    @namespace = "fnordmetric-test-ns1234-api" 
    @timeline = "#{@namespace}-timeline"

    @opts = {         
      :namespace_prefix => "#{@namespace}",
      :redis_prefix => "fnordmetric-test",
      :redis => @redis
    }  
    @api = API.new @opts
  end

  describe "finding events using API" do

    before(:each) do        
      @redis.keys("fnordmetric-test-*").each { |k| @redis.del(k) }     
    end

    it "should find all events" do
      create_event({:_type => "foo", :_time => @now})
      create_event({:_type => "foo", :_time => @now})
      Event.all(@opts).length.should == 2
    end

    it "should find all events and return event objects" do
      create_event({:_type => "foo", :_time => @now})
      Event.all(@opts).first.should be_a(FnordMetric::Event)
    end

    it "should find all events and returnevent objects with time" do
      create_event({:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      events = Event.all(@opts) 
      events.first.time.to_i.should == @now
    end

    it "should find an event and return a event object" do
      event_id = create_event({:_type => "Fn0rd", :_time => @now})
      event = Event.find(event_id, @opts)
      event.should be_a(FnordMetric::Event)
      event.type.should == "Fn0rd"
    end

    it "should find an event and return a event object with data" do
      event_id = create_event({:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      event = Event.find(event_id, @opts)  
      event.data(:blah).should == "blubb"
    end

    it "should find an event and return a event object with id" do
      event_id = create_event({:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      event = Event.find(event_id, @opts) 
      event.id.should == event_id
    end

    it "should find all in the correct order" do
      create_event({:_type => "foo", :_time => @now-17})
      create_event({:_type => "foo", :_time => @now-23})
      create_event({:_type => "foo", :_time => @now-42})
      create_event({:_type => "foo", :_time => @now-5})
      Event.all(@opts).length.should == 4
    end

    it "should find all events since a given time, including that exact time" do
      create_event({:_type => "foo", :_time => @now-42})
      create_event({:_type => "foo", :_time => @now-23})
      create_event({:_type => "foo", :_time => @now-17})
      create_event({:_type => "foo", :_time => @now-5})
      Event.all(@opts).length.should == 4
      Event.all(@opts.merge(:since => @now-42)).length.should == 4
      Event.all(@opts.merge(:since => @now-23)).length.should == 3
      Event.all(@opts.merge(:since => @now-22)).length.should == 2
      Event.all(@opts.merge(:since => @now-17)).length.should == 2
      Event.all(@opts.merge(:since => @now-16)).length.should == 1
      Event.all(@opts.merge(:since => @now-5)).length.should == 1
    end

    it "should find a maximum number of events" do
      create_event( {:_type => "foo", :_time => @now-42})
      create_event( {:_type => "foo", :_time => @now-23})
      create_event( {:_type => "foo", :_time => @now-17})
      create_event( {:_type => "foo", :_time => @now-5})
      Event.all(@opts).length.should == 4
      Event.all(@opts.merge(:limit => 2)).length.should == 2
    end



    def create_event(event_data)
      event_id = @api.event event_data.to_json
      @redis_wrap.zadd(@timeline, event_data.delete(:_time), event_id)
      event_id
    end

  end


end
