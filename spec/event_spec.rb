require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Event do

  include FnordMetric

  before(:all) do
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)

    @namespace = "fnordmetric-test-ns123" 
    @timeline = "#{@namespace}-timeline"

    @opts = {         
      :namespace_prefix => "#{@namespace}",
      :redis => @redis
    }  
  end

  describe "finding events" do

    before(:each) do        
      @redis.keys("fnordmetric-test-*").each { |k| @redis.del(k) }     
    end

    it "should find all events" do
      create_event("242342", {:_type => "foo", :_time => @now})
      create_event("453484", {:_type => "foo", :_time => @now})
      Event.all(@opts).length.should == 2
    end

    it "should find all events and return event objects" do
      create_event("434454", {:_type => "foo", :_time => @now})
      Event.all(@opts).first.should be_a(FnordMetric::Event)
    end

    it "should find an event and return a event object" do
      create_event("756753", {:_type => "Fn0rd", :_time => @now})
      event = Event.find("756753", @opts)
      event.should be_a(FnordMetric::Event)
      event.type.should == "Fn0rd"
    end

    it "should find an event and return a event object with data" do
      create_event("53454", {:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      event = Event.find("53454", @opts)  
      event.data(:blah).should == "blubb"
    end

    it "should find an event and return a event object with id" do
      create_event("5262435", {:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      event = Event.find("5262435", @opts) 
      event.id.should == "5262435"
    end

    it "should find an event and return a event object with time" do
      create_event("352234", {:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      event = Event.find("352234", @opts) 
      event.time.should == @now
    end

    def create_event(event_id, event_data)        
      @redis_wrap.zadd(@timeline, event_data.delete(:_time), event_id)
      @redis.set(event_id, event_data.to_json)        
    end

  end


end