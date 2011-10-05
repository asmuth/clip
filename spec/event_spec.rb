require ::File.expand_path('../spec_helper.rb', __FILE__)

include Fnordmetric

describe "event" do

  before(:each) do
    Event.destroy_all
  end

  it "should create a new data point" do
    event = Event.track!(:type => '_referral', :foobar => "fnord")
    puts Event.last.inspect
    Event.last[:type].should == "_referral"
    Event.last[:foobar].should == "fnord"
  end

  it "should access info like a 'ostruct' object" do
    event = Event.track!(:type => '_referral', :foobar => "fnord")
    Event.last.type.should == "_referral"
    Event.last.foobar.should == "fnord"
  end

  it "should insert a data point in the past" do
    my_time = 23.minutes.ago
    event = Event.track!(:type => '_referral', :foobar => "fnord", :time => my_time)
    Event.last[:type].should == "_referral"
    Event.last[:foobar].should == "fnord"
    Event.last[:time].should == my_time.to_i
  end
    
end