require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::TimeseriesGauge do

  before(:all) do    
    @now = Time.utc(1992,01,13,5,23,23).to_i    
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis, false)
    @gauge_klass = FnordMetric::TimeseriesGauge
  end

  before(:each) do
    @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
  end

  it_should_behave_like FnordMetric::GaugeLike

  describe "option validation" do

    it "should raise when initialized with non-unique series tokens" do
      pending "fix me"
      lambda{
        FnordMetric::TimeseriesGauge.new(
          :series => [:fnord, :fnord], 
          :ticks => [1.hour],
          :key => 123, 
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized without series tokens key" do
      lambda{
        FnordMetric::TimeseriesGauge.new(
          :key => 123, 
          :ticks => [1.hour],
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized with emtpy series tokens" do
      pending "fix me"
      lambda{
        FnordMetric::TimeseriesGauge.new(
          :series => [],
          :key => 123,
          :ticks => [1.hour],
          :key_prefix => 123
        )
      }.should raise_exception
    end

  end

end
