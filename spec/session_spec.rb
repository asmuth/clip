require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Session do

  include FnordMetric

  before(:all) do
    @now = Time.utc(1992,01,13,5,23,23).to_i   
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)

    @namespace = "fnordmetric-ns123" 
    @sessions = "#{@namespace}-session"
  end

  describe "creating sessions" do

    before(:all) do
      @event = { :_session => "sess123", :_time => @now, :_eid => "34089749" }
      @md5_key = Digest::MD5.hexdigest("sess123")          

      @redis.del("#{@namespace}-session")           
      @redis.del("#{@namespace}-session-#{@md5_key}-events")
      @redis.del("#{@namespace}-session-#{@md5_key}-data")
    end

    it "should add a new session on intialize" do        
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => @event, 
        :session_data_ttl => 10,
        :redis => @redis_wrap        
      )
      @redis.zcard(@sessions).should == 1
    end

    it "should add a new session on intialize and hash the session token" do               
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => @event, 
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      @redis.zrange(@sessions, 0, -1).should == [ @md5_key]
    end

    it "should add a new session on intialize and set the timestamp as score" do               
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => @event, 
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      @redis.zscore(@sessions, @md5_key).to_i.should == @now
    end

    it "should update the timestamp on a existing session" do     
      @redis.zadd(@sessions, @now-10, @md5_key)
      @redis.zscore(@sessions, @md5_key).to_i.should == @now-10
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => @event, 
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      @redis.zscore(@sessions, @md5_key).to_i.should == @now
    end

    it "should add the event_id to the session-event set on a new session" do               
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => @event, 
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      events_key = "#{@namespace}-session-#{@md5_key}-events"
      @redis.zrange(events_key, 0, -1).first.should == @event[:_eid]
    end

    it "should store a name in the session data" do   
      event_data = @event.merge(
        :_type => "_set_name", 
        :name => "Horst Mayer"
      )            
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => event_data,
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      data_key = "#{@namespace}-session-#{@md5_key}-data"
      @redis.hget(data_key, "_name").should == "Horst Mayer"
    end

    it "should store a picture in the session data" do               
      event_data = @event.merge(
        :_type => "_set_picture", 
        :url => "http://myhost/mypic.jpg"
      )
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => event_data,
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      data_key = "#{@namespace}-session-#{@md5_key}-data"
      @redis.hget(data_key, "_picture").should == "http://myhost/mypic.jpg"
    end

    it "should store arbitrary data in the session data" do               
      event_data = @event.merge(
        :_type => "_set_data", 
        :fnord => "blubb", 
        :foobar => "123"
      )
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => event_data,
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      data_key = "#{@namespace}-session-#{@md5_key}-data"
      @redis.hget(data_key, "fnord").should == "blubb"
      @redis.hget(data_key, "foobar").should == "123"
    end

    it "not store special attributes in the session" do               
      event_data = @event.merge(
        :_type => "_set_data", 
        :fnord => "blubb", 
        :foobar => "123"
      )
      FnordMetric::Session.create(
        :namespace_prefix => @namespace,
        :event => event_data,
        :session_data_ttl => 10,
        :redis => @redis_wrap
      )    
      data_key = "#{@namespace}-session-#{@md5_key}-data"
      @redis.hget(data_key, "_time").should be_nil
      @redis.hget(data_key, "_eid").should be_nil
    end

  end

  describe "Finding Sessions" do

    before(:each) do      
      @redis.del("#{@namespace}-session")           
      @redis.keys("#{@namespace}-session-*").each { |k| @redis.del(k) }     

      @opts = {         
        :redis_prefix => "#{@namespace}-session",
        :namespace_prefix => "#{@namespace}",
        :redis => @redis
      }  
    end

    it "should find all sessions" do
      create_session("sess533", @now, {})
      create_session("sess343", @now, {})
      FnordMetric::Session.all(@opts).length.should == 2
    end

    it "should find all sessions and return session objects" do
      create_session("sess523", @now, {})
      FnordMetric::Session.all(@opts).first.should be_a(FnordMetric::Session)
    end

    it "should find a session and return a session object" do
      create_session("sess223", @now, {})
      sess = FnordMetric::Session.find(Digest::MD5.hexdigest("sess223"), @opts)
      sess.should be_a(FnordMetric::Session)
      sess.session_key.should == Digest::MD5.hexdigest("sess223")
    end

    it "should find a sessions and return a session object with data" do
      create_session("sess123", @now, { :fnord => "blubb" })
      sess = FnordMetric::Session.find(Digest::MD5.hexdigest("sess123"), @opts)  
      sess.fetch_data!    
      sess.data(:fnord).should == "blubb"
    end

    it "should not include special attributes in data" do
      event_data = { :_name => "Horst Mayer", :_picture => "http://myhost/mypic.jpg" }
      create_session("sess173", @now, event_data)
      sess = FnordMetric::Session.find(Digest::MD5.hexdigest("sess173"), @opts)      
      sess.fetch_data!
      sess.data(:_name).should == nil
      sess.data(:_picture).should == nil
    end

    it "should find a session and return a session object with picture" do
      event_data = { :_name => "Horst Mayer", :_picture => "http://myhost/mypic.jpg" }
      create_session("sess163", @now, event_data)
      sess = FnordMetric::Session.find(Digest::MD5.hexdigest("sess163"), @opts)      
      sess.fetch_data!
      sess.picture.should == "http://myhost/mypic.jpg"
    end

    it "should find a session and return a session object with name" do
      event_data = { :_name => "Horst Mayer", :_picture => "http://myhost/mypic.jpg" }
      create_session("sess143", @now, event_data)
      sess = FnordMetric::Session.find(Digest::MD5.hexdigest("sess143"), @opts)      
      sess.fetch_data!
      sess.name.should == "Horst Mayer"
    end

    it "should find a session and return a session object with event_ids" do
      sesshash = create_session("sess923", @now, {})
      @redis_wrap.zadd("#{@namespace}-session-#{sesshash}-events", @now, "shmoo")       
      @redis_wrap.zadd("#{@namespace}-session-#{sesshash}-events", @now, "fnord")
      sess = FnordMetric::Session.find(sesshash, @opts)
      sess.fetch_event_ids!
      sess.event_ids.should include("shmoo")
      sess.event_ids.should include("fnord")
    end

    def create_session(sesskey, sesstime, sessdata)        
      Digest::MD5.hexdigest(sesskey).tap do |sesshash|
        @redis_wrap.zadd("#{@namespace}-session", sesstime, sesshash)        
        sessdata.each do |k,v|
          @redis_wrap.hset("#{@namespace}-session-#{sesshash}-data", k, v)        
        end
      end
    end

  end


end
