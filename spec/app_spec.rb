require ::File.expand_path('../spec_helper.rb', __FILE__)

include Rack::Test::Methods

describe "app" do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  def app
    @app ||= FnordMetric::App
  end

  it "should render something on /" do
  	get "/"
  	last_response.status.should == 200
  end

  it "should track an event without auth" do
  	post "/events", :type => "myevent", :fnord => "foobar"
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.fnord.should == "foobar"
  end

  it "should return 400 if no type is provided" do
  	post "/events", :fnord => "foobar"
  	last_response.status.should == 400  	
  	last_response.body.should == "please specify the event_type"
  end

  it "should track an event in the past" do
  	my_time = (Time.now-3.years).to_i
  	post "/events", :type => "myevent", :time => my_time
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.time.should == my_time
  end

  it "should track an event with integer data" do
  	post "/events", :type => "myevent", :blubb => "123"
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.blubb.should == 123
  end
  
  it "should track an event with float data" do
  	post "/events", :type => "myevent", :blubb => "42.23"
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.blubb.should == 42.23
  end

end