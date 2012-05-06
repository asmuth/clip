require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::MultiGauge do

  before(:all) do    
    @now = Time.utc(1992,01,13,5,23,23).to_i    
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis, false)
    @gauge_klass = FnordMetric::MultiGauge
  end

  before(:each) do
    @redis.keys("fnordmetric-myns*").each { |k| @redis.del(k) }  
  end

  it_should_behave_like FnordMetric::GaugeLike

end
