require ::File.expand_path('../spec_helper.rb', __FILE__)

include Fnordmetric

describe "event" do

  before(:each) do
    Event.destroy_all
  end

  it "should create a new data point" do
    event = Event.track!('_referral', :foobar => "fnord")
    Event.last[:type].should == "_referral"
    Event.last[:foobar].should == "fnord"
  end

  it "should access info like a 'ostruct' object" do
    event = Event.track!('_referral', :foobar => "fnord")
    Event.last.type.should == "_referral"
    Event.last.foobar.should == "fnord"
  end

  it "should insert a data point in the past" do
    my_time = 23.minutes.ago
    event = Event.track!('_referral', :foobar => "fnord", :time => my_time)
    Event.last[:type].should == "_referral"
    Event.last[:foobar].should == "fnord"
    Event.last[:time].should == my_time.to_i
  end

  it "should save integer data" do
    event = Event.track!('_test', :num => 23)
    Event.last.num.should == 23
  end

  it "should save float data" do
    event = Event.track!('_test', :num => 42.5)
    Event.last.num.should == 42.5
  end
  
end