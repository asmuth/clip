require ::File.expand_path('../spec_helper.rb', __FILE__)

include Rack::Test::Methods

describe "app" do

  before(:all) do
    @opts = {}
  end

  def app
    namespaces =     
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil
      }
    }, @opts)
  end

  it "should redirect to the first namespace" do
  	get "/"
  	last_response.status.should == 302
  	last_response.location.should == "http://example.org/foospace"
  end

  it "should render the namespace" do
    get "/foospace"
    last_response.status.should == 200
  end

  it "should render the dashboards" do
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil
      }
    }, @opts)
  	get "/foospace"
  	last_response.status.should == 200
    last_response.body.should include("Blubb")
  end

  it "should render the dashboards" do
    @app ||= FnordMetric::App.new({
      :foospace => proc{        
        widget 'Blubb', nil
        widget 'Fnord', nil
      }
    }, @opts)
    get "/foospace"
    last_response.status.should == 200
    last_response.body.should include("Blubb")
    last_response.body.should include("Fnord")
  end


  describe "events api" do

    it "should track an event without auth" do
      pending("fix this")
      #post "/events", :type => "myevent", :fnord => "foobar"
      #last_response.status.should == 200
      #FnordMetric::Event.last.type.should == "myevent"
      #FnordMetric::Event.last.fnord.should == "foobar"
    end

    it "should return 400 if no type is provided" do
      pending("fix this")
    	#post "/events", :fnord => "foobar"
    	#last_response.status.should == 400  	
    	#last_response.body.should == "please specify the event_type"
    end

    it "should track an event in the past" do
      pending("fix this")
    	#my_time = (Time.now-3.years).to_i
    	#post "/events", :type => "myevent", :time => my_time
    	#last_response.status.should == 200
    	#FnordMetric::Event.last.type.should == "myevent"
    	#FnordMetric::Event.last.time.should == my_time
    end

    it "should track an event with integer data" do
      pending("fix this")
    	#post "/events", :type => "myevent", :blubb => "123"
    	#last_response.status.should == 200
    	#FnordMetric::Event.last.type.should == "myevent"
    	#FnordMetric::Event.last.blubb.should == 123
    end
    
    it "should track an event with float data" do
      pending("fix this")
    	#post "/events", :type => "myevent", :blubb => "42.23"
    	#last_response.status.should == 200
    	#FnordMetric::Event.last.type.should == "myevent"
    	#FnordMetric::Event.last.blubb.should == 42.23
    end

  end

  describe "metrics api" do
    # copy from _spec/app_spec.rb ?
  end


end