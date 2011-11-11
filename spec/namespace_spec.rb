require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Namespace do

  it "should generate the correct redis prefix"

  it "should register a gauge"
  it "should register a gauge and pass options"

  it "should register an event handler"
  it "should register an event handler and create a context"
  it "should register an event handler and pass options"
  it "should register an event handler and pass gauges"

  it "should announce an event to the correct handler" do
    pending("finish this")
  # block_called = false
  # FnordMetric::Dashboard.new(:title => 'My Dashboard') do |dash|
  #    block_called = true
  #    dash.should be_a(FnordMetric::Dashboard)
  #  end
  #  block_called.should be_true
  end

  it "should announce an event to multiple handlers"

end