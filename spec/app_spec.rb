require ::File.expand_path('../spec_helper.rb', __FILE__)

include Rack::Test::Methods

describe "app" do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  def app
    @app ||= FnordMetric::App
  end

  it "should redirect to '/fnordmetric'" do
  	get "/"
  	last_response.status.should == 302
  	last_response.location.should == "http://example.org/fnordmetric"
  end

  it "should redirect to the default dashboard" do
  	get "/fnordmetric"
  	last_response.status.should == 302
  	last_response.location.should == "http://example.org/fnordmetric/dashboard/default"
  end

  it "should render the dashboard" do
  	get "/fnordmetric/dashboard/default"
  	last_response.status.should == 200
  end

  it "should render the right dashboard" do
    FnordMetric.dashboard("Deine Mama"){|dash|}
    FnordMetric.dashboard("My Dashboard"){|dash|}
    get "/fnordmetric/dashboard/DeineMama"
    last_response.status.should == 200
    last_response.body.should include("Deine Mama")
    get "/fnordmetric/dashboard/MyDashboard"
    last_response.status.should == 200
    last_response.body.should include("My Dashboard")
  end

  it "should track an event without auth" do
  	post "/fnordmetric/events", :type => "myevent", :fnord => "foobar"
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.fnord.should == "foobar"
  end

  it "should return 400 if no type is provided" do
  	post "/fnordmetric/events", :fnord => "foobar"
  	last_response.status.should == 400  	
  	last_response.body.should == "please specify the event_type"
  end

  it "should track an event in the past" do
  	my_time = (Time.now-3.years).to_i
  	post "/fnordmetric/events", :type => "myevent", :time => my_time
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.time.should == my_time
  end

  it "should track an event with integer data" do
  	post "/fnordmetric/events", :type => "myevent", :blubb => "123"
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.blubb.should == 123
  end
  
  it "should track an event with float data" do
  	post "/fnordmetric/events", :type => "myevent", :blubb => "42.23"
  	last_response.status.should == 200
  	FnordMetric::Event.last.type.should == "myevent"
  	FnordMetric::Event.last.blubb.should == 42.23
  end


  describe "metrics api" do

    before(:each) do
      FnordMetric::Event.destroy_all
      FnordMetric.metric('my_event_count', :count => true, :types => [:my_event_type])
      FnordMetric.track('my_event_type', :time => 33.hours.ago)
      FnordMetric.track('my_event_type', :time => 32.hours.ago)
      FnordMetric.track('my_event_type', :time => 29.hours.ago)
      FnordMetric.track('my_event_type', :time => 27.hours.ago)
      FnordMetric.track('my_event_type', :time => 26.hours.ago)
      FnordMetric.track('my_event_type', :time => 13.hours.ago)
      FnordMetric.track('my_event_type', :time => 12.hours.ago)
      FnordMetric.track('my_event_type', :time => 2.hours.ago)      
      FnordMetric.track('my_event_type', :time => 3.hours.ago)  
    end

    it "should return the right answer for: /metric/:name" do      
      get "/fnordmetric/metric/my_event_count"
      JSON.parse(last_response.body)["value"].to_i.should == 9      
    end

    it "should return the right answer for: /metric/:name?at=timestamp" do      
      get "/fnordmetric/metric/my_event_count", :at => 18.hours.ago.to_i.to_s
      JSON.parse(last_response.body)["value"].to_i.should == 5
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp" do      
      get "/fnordmetric/metric/my_event_count", :at => "#{30.hours.ago.to_i}-#{20.hours.ago.to_i}"
      JSON.parse(last_response.body)["value"].to_i.should == 3
    end   

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i, :delta => true
      JSON.parse(last_response.body)["values"].length.should == 6
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i, :delta => true
      JSON.parse(last_response.body)["values"][0].first.should == 34.hours.ago.to_i
      JSON.parse(last_response.body)["values"][0].last.should == 3
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i, :delta => true
      JSON.parse(last_response.body)["values"][1].first.should == 28.hours.ago.to_i
      JSON.parse(last_response.body)["values"][1].last.should == 2
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i, :delta => true
      JSON.parse(last_response.body)["values"][2].first.should == 22.hours.ago.to_i
      JSON.parse(last_response.body)["values"][2].last.should == 0
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i, :delta => true
      JSON.parse(last_response.body)["values"][3].first.should == 16.hours.ago.to_i
      JSON.parse(last_response.body)["values"][3].last.should == 2
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i, :delta => true
      JSON.parse(last_response.body)["values"][5].first.should == 4.hours.ago.to_i
      JSON.parse(last_response.body)["values"][5].last.should == 2
    end



    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i
      JSON.parse(last_response.body)["values"].length.should == 6
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i
      JSON.parse(last_response.body)["values"][0].first.should == 34.hours.ago.to_i
      JSON.parse(last_response.body)["values"][0].last.should == 0
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i
      JSON.parse(last_response.body)["values"][1].first.should == 28.hours.ago.to_i
      JSON.parse(last_response.body)["values"][1].last.should == 3
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i
      JSON.parse(last_response.body)["values"][2].first.should == 22.hours.ago.to_i
      JSON.parse(last_response.body)["values"][2].last.should == 5
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i
      JSON.parse(last_response.body)["values"][3].first.should == 16.hours.ago.to_i
      JSON.parse(last_response.body)["values"][3].last.should == 5
    end

    it "should return the right answer for: /metric/:name?at=timestamp-timstamp&tick=seconds" do
      get "/fnordmetric/metric/my_event_count", :at => "#{34.hours.ago.to_i}-#{1.hour.ago.to_i}", :tick => 6.hours.to_i
      JSON.parse(last_response.body)["values"][5].first.should == 4.hours.ago.to_i
      JSON.parse(last_response.body)["values"][5].last.should == 7
    end

  end
 
end