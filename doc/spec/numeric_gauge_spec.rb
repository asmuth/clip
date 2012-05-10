require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::NumericGauge do

  before(:all) do    
    @now = Time.utc(1992,01,13,5,23,23).to_i    
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis, false)
    @gauge_klass = FnordMetric::NumericGauge
  end

  before(:each) do
    @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
  end

  it_should_behave_like FnordMetric::GaugeLike

  describe "option validation" do

    it "should raise when initialized with non-unique series tokens" do
      lambda{
        FnordMetric::NumericGauge.new(
          :series => [:fnord, :fnord], 
          :ticks => [1.hour],
          :key => 123, 
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized without series tokens key" do
      lambda{
        FnordMetric::NumericGauge.new(
          :key => 123, 
          :ticks => [1.hour],
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized with emtpy series tokens" do
      lambda{
        FnordMetric::NumericGauge.new(
          :series => [],
          :key => 123,
          :ticks => [1.hour],
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized without ticks key" do
      lambda{
        FnordMetric::NumericGauge.new(
          :key => 123, 
          :series => [:fu],
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized with emtpy ticks" do
      lambda{
        FnordMetric::NumericGauge.new(
          :ticks => [],
          :series => [:fu],
          :key => 123, 
          :key_prefix => 123
        )
      }.should raise_exception
    end

    it "should raise when initialized with invalid ticks" do
      lambda{
        FnordMetric::NumericGauge.new(
          :ticks => ["fnord"],
          :series => ["fu"],
          :key => 123, 
          :key_prefix => 123
        )
      }.should raise_exception
    end

  end

end
