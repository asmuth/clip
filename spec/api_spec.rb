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
    @api = FnordMetric::API.new @opts
  end

  describe "creating events using API" do

    before(:each) do        
      @redis.keys("fnordmetric-test-*").each { |k| @redis.del(k) }     
    end

    it "should create an event from a (json-)string" do
      json_string = {
        :_type => "Fn0rd123", 
        :_time => @now
      }.to_json
      event_id = @api.event(json_string)
      event = FnordMetric::Event.find(event_id, @opts)
      event.type.should == "Fn0rd123"
    end

    it "should create an event from a hash" do
      event_id = @api.event(
        :_type => "Fn0rd234", 
        :_time => @now
      )
      event = FnordMetric::Event.find(event_id, @opts)
      event.type.should == "Fn0rd234"
    end

  end
end
