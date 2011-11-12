require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Session do

  include FnordMetric

  before(:all) do
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)

    @event = { :_session => "sess123", :_time => @now, :_eid => "34089749" }
    @md5_key = Digest::MD5.hexdigest("sess123")    
    @namespace = "fnordmetric-ns123" 
    @sessions = "#{@namespace}-sessions"

    @redis.del("#{@namespace}-sessions")           
    @redis.del("#{@namespace}-sessions-#{@md5_key}-events")
    @redis.del("#{@namespace}-sessions-#{@md5_key}-data")
  end

  it "should add a new session on intialize" do        
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @redis_wrap
    )
    @redis.zcard(@sessions).should == 1
  end

  it "should add a new session on intialize and hash the session token" do               
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @redis_wrap
    )    
    @redis.zrange(@sessions, 0, -1).should == [ @md5_key]
  end

  it "should add a new session on intialize and set the timestamp as score" do               
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @redis_wrap
    )    
    @redis.zscore(@sessions, @md5_key).to_i.should == @now
  end

  it "should update the timestamp on a existing session" do     
    @redis.zadd(@sessions, @now-10, @md5_key)
    @redis.zscore(@sessions, @md5_key).to_i.should == @now-10
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @redis_wrap
    )    
    @redis.zscore(@sessions, @md5_key).to_i.should == @now
  end

  it "should add the event_id to the session-event list on a new session" do               
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @redis_wrap
    )    
    events_key = "#{@namespace}-sessions-#{@md5_key}-events"
    @redis.lrange(events_key, 0, 0).first.should == @event[:_eid]
  end

  it "should store a name in the session data" do   
    event_data = @event.merge(
      :type => "_set_picture", 
      :name => "Horst Mayer"
    )            
    Session.new(
      :namespace_prefix => @namespace,
      :event => event_data,
      :redis => @redis_wrap
    )    
    data_key = "#{@namespace}-sessions-#{@md5_key}-data"
    @redis.hget(data_key, "_name").should == "Horst Mayer"
  end

  it "should store a picutre in the session data" do               
    event_data = @event.merge(
      :type => "_set_picture", 
      :picture => "http://myhost/mypic.jpg"
    )
    Session.new(
      :namespace_prefix => @namespace,
      :event => event_data,
      :redis => @redis_wrap
    )    
    data_key = "#{@namespace}-sessions-#{@md5_key}-data"
    @redis.hget(data_key, "_picture").should == "http://myhost/mypic.jpg"
  end

  it "should store arbitrary data in the session data" do               
    event_data = @event.merge(
      :type => "_set_data", 
      :fnord => "blubb", 
      :foobar => "123"
    )
    Session.new(
      :namespace_prefix => @namespace,
      :event => event_data,
      :redis => @redis_wrap
    )    
    data_key = "#{@namespace}-sessions-#{@md5_key}-data"
    @redis.hget(data_key, "fnord").should == "blubb"
    @redis.hget(data_key, "foobar").should == "123"
  end

  it "not store special attributes in the session" do               
    event_data = @event.merge(
      :type => "_set_data", 
      :fnord => "blubb", 
      :foobar => "123"
    )
    Session.new(
      :namespace_prefix => @namespace,
      :event => event_data,
      :redis => @redis_wrap
    )    
    data_key = "#{@namespace}-sessions-#{@md5_key}-data"
    @redis.hget(data_key, "_time").should be_nil
    @redis.hget(data_key, "_eid").should be_nil
  end



end