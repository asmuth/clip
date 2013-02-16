require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Worker do

  let(:worker) { FnordMetric::Worker.new() }

  it "should generate the correct pubsub-key" do
    worker.pubsub_key.should == "fnordmetric-announce"
  end

  it "should generate the correct queue-key" do
    worker.queue_key.should == "fnordmetric-queue"
  end

  it "should generate the correct event-key" do
    worker.event_key("myevent").should == "fnordmetric-event-myevent"
  end

  it "should generate the correct stats-key" do
    worker.stats_key.should == "fnordmetric-stats"
  end

  it "should add a namespace"  
  it "should add a namespace and pass options"  
  it "should add a namespace and the key"  
  it "should add a namespace and instance_eval the block"
      
  it "should find a namespace by key"
  it "should find the namespace default namespace without key"

end

