require ::File.expand_path('../spec_helper.rb', __FILE__)
require 'irb'

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
      :redis_prefix => "fnordmetric-test",
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

    it "should find all events and returnevent objects with time" do
      create_event("352234", {:_type => "Fn0rd", :blah => :blubb, :_time => @now})
      events = Event.all(@opts) 
      events.first.time.to_i.should == @now
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

    it "should find all in the correct order" do
      create_event("5645642", {:_type => "foo", :_time => @now-17})
      create_event("2342366", {:_type => "foo", :_time => @now-23})
      create_event("3452345", {:_type => "foo", :_time => @now-42})
      create_event("6345345", {:_type => "foo", :_time => @now-5})
      Event.all(@opts).length.should == 4
      Event.all(@opts)[0].id.should == "6345345"
      Event.all(@opts)[1].id.should == "5645642"
      Event.all(@opts)[2].id.should == "2342366"
      Event.all(@opts)[3].id.should == "3452345"
    end

    it "should find all events since a given time, including that exact time" do
      create_event("3452345", {:_type => "foo", :_time => @now-42})
      create_event("2342366", {:_type => "foo", :_time => @now-23})
      create_event("5645642", {:_type => "foo", :_time => @now-17})
      create_event("6345345", {:_type => "foo", :_time => @now-5})
      Event.all(@opts).length.should == 4
      Event.all(@opts.merge(:since => @now-42)).length.should == 4
      Event.all(@opts.merge(:since => @now-23)).length.should == 3
      Event.all(@opts.merge(:since => @now-22)).length.should == 2
      Event.all(@opts.merge(:since => @now-17)).length.should == 2
      Event.all(@opts.merge(:since => @now-16)).length.should == 1
      Event.all(@opts.merge(:since => @now-5)).length.should == 1
    end

    it "should find a maximum number of events" do
      create_event("3452345", {:_type => "foo", :_time => @now-42})
      create_event("2342366", {:_type => "foo", :_time => @now-23})
      create_event("5645642", {:_type => "foo", :_time => @now-17})
      create_event("6345345", {:_type => "foo", :_time => @now-5})
      Event.all(@opts).length.should == 4
      Event.all(@opts.merge(:limit => 2)).length.should == 2
      Event.all(@opts.merge(:limit => 2)).first.id.should == "6345345"
      Event.all(@opts.merge(:limit => 2)).last.id.should == "5645642"
    end

    it "should find all events by type" do
      namespace = Namespace.new(:ns123, :redis_prefix => "fnordmetric-test")
      namespace.ready!(@redis_wrap, @redis).announce(
        :_type => "fn0rd",
        :_time => @now
      )
      namespace.ready!(@redis_wrap, @redis).announce(
        :_type => "f00bar",
        :_time => @now
      )
      namespace.ready!(@redis_wrap, @redis).announce(
        :_type => "fn0rd",
        :_time => @now
      )
      Event.by_type('fn0rd', @opts).length.should == 2
      Event.by_type('f00bar', @opts).length.should == 1
    end

    describe "looking by session_key" do
      let(:namespace) do
        options = {:redis_prefix => "fnordmetric-test", :session_data_ttl => 1}
        Namespace.new(:ns123, options)
      end

      let(:events_data) do
        [[1, "max_session"], [2, "kate_session"], [3, "kate_session"], [12345678, nil]]
      end

      let(:kate_session_key) { Digest::MD5.hexdigest "kate_session" }
      let(:events) { Event.by_session_key kate_session_key, @opts }

      before do
        created_events_data.each do |(event_id, session)|
          event_data = { :_time => @now + event_id, :_eid => event_id, :_type => "fnord" }
          event_data[:_session] = session if session
          namespace.ready!(@redis_wrap, @redis).announce event_data
        end
      end

      context "when events are added in ascending time order" do
        let(:created_events_data) { events_data }

        it "should find correct events" do
          ["2","3"].each {|id| events.map(&:id).should include(id) }
        end

        it "should have correct order" do
          events.map(&:id).should == ["3","2"]
        end
      end

      context "when events are added in descending time order" do
        let(:created_events_data) { events_data.reverse }

        it "should have correct order" do
          events.map(&:id).should == ["3","2"]
        end
      end
    end

    def create_event(event_id, event_data)
      @redis_wrap.zadd(@timeline, event_data.delete(:_time), event_id)
      @redis.set("fnordmetric-test-event-#{event_id}", event_data.to_json)        
    end

  end


end
