require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Session do

  include FnordMetric

  before(:all) do
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)

    @event = { :_session => "sess123", :_time => @now }
    @md5_key = Digest::MD5.hexdigest("sess123")    
    @namespace = "fnordmetric-ns123" 
    @sessions = "#{@namespace}-sessions"
    @redis.del("#{@namespace}-sessions")           
  end

  it "should add a new session on intialize" do        
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @rediswrap
    )
    @redis.zcard(@sessions).should == 1
  end

  it "should add a new session on intialize and hash the session token" do               
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @rediswrap
    )    
    @redis.zrange(@sessions, 0, -1).should == [ @md5_key]
  end

  it "should add a new session on intialize and set the timestamp as score" do               
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @rediswrap
    )    
    @redis.zscore(@sessions, @md5_key).should == [ @md5_key ]
  end

  it "should update the timestamp on a existing session" do     
    @redis.zadd(@sessions, @now-10, @md5_key)
    @redis.zscore(@sessions, @md5_key).to_i.should == @now-10
    Session.new(
      :namespace_prefix => @namespace,
      :event => @event, 
      :redis => @rediswrap
    )    
    @redis.zscore(@sessions, @md5_key).to_i.should == @now
  end

end