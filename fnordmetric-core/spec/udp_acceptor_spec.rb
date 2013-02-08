require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::UDPAcceptor do

  let(:inbound_datagram) { FnordMetric::UDPAcceptor.new(nil) }

  before(:all) do
    @redis = Redis.new
    @redis_wrap = RedisWrap.new(@redis)
    FnordMetric::UDPAcceptor.opts = {
      :redis_url => "redis://localhost:6379",
      :redis_prefix => "fnordmetric-test",
      :event_queue_ttl => 120
    }
  end

  describe "pushing new events" do
    it "should add parsable event to the queue" do
      pending "fixme"
      data = %Q{{"_type": "started"}}

      lambda {
        inbound_datagram.receive_data data
      }.should change { @redis.llen("fnordmetric-test-queue") }.by +1
    end

    it "should reject non parsable events" do
      broken_data = %Q{{"_type" => "started"}}

      lambda {
        inbound_datagram.receive_data broken_data
      }.should_not change { @redis.llen("fnordmetric-test-queue") }
    end
  end
end
