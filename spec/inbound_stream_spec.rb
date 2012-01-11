require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::InboundStream do
  class TestStream
    module TestEM
      def self.defer; yield; end
      def self.next_tick; yield; end
    end

    FnordMetric::InboundStream::Logic.em = TestEM
    FnordMetric::InboundStream::Logic.opts =
      {redis_url: "redis://localhost:6379",
        redis_prefix: "fnordmetric-test", event_queue_ttl: 120}

    include FnordMetric::InboundStream::Logic
  end

  let(:inbound_stream) do
    TestStream.new.tap do |logic|
      logic.post_init
    end
  end

  let(:redis) { inbound_stream.instance_variable_get("@redis") }

  after { inbound_stream.unbind }

  describe "pushing new events" do
    it "should add parsable event to the queue" do
      data = %Q{{"_type": "started"}\n}

      lambda {
        inbound_stream.receive_data data
      }.should change { redis.llen("fnordmetric-test-queue") }.by +1
    end

    it "should reject non parsable events" do
      broken_data = %Q{{"_type": \n"started"}\n}

      lambda {
        inbound_stream.receive_data broken_data
      }.should_not change { redis.llen("fnordmetric-test-queue") }
    end
  end
end

