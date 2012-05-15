require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Widget do

  before(:each) do
    @gauge1 = FnordMetric::Gauge.new(
      :key => "foooobar",
      :tick => 60*60*24,
      :key_prefix => "blubb"
    )
    @gauge2 = FnordMetric::Gauge.new(
      :key => "fooo234obar",
      :tick => 60*60,
      :key_prefix => "blubb"
    )
  end

  it "should remember it's own title" do
    widget = FnordMetric::Widget.new(
      :title => "My Widget",
      :gauges => [@gauge1]
    )
    widget.title.should == "My Widget"
  end
  
  it "should remember it's gauges" do
    widget = FnordMetric::Widget.new(
      :title => "My Widget",
      :gauges => [@gauge1]
    )
    widget.gauges.should == [@gauge1]
  end
  
  it "should calculate the correct tick" do
    widget = FnordMetric::Widget.new(
      :title => "My Widget",
      :gauges => [@gauge1]
    )
    widget.tick.should == 60*60*24
  end
  
  it "should raise an error if two gauges with different ticks are added" do
    lambda{
      widget = FnordMetric::TimelineWidget.new(
        :title => "My Widget",
        :gauges => [@gauge1, @gauge2]
      ) 
    }.should raise_error
  end

  it "should generate the correct default range for daily graphs without include current" do
    widget = FnordMetric::TimelineWidget.new(
      :title => "My Widget",
      :include_current => false,
      :gauges => [@gauge1]
    ) 
    range = widget.default_range(Time.utc(1992,01,13,18,23,23))
    Time.at(range.last).utc.should == Time.utc(1992,01,12,00,00)
    Time.at(range.first).utc.should == Time.utc(1991,12,13,00,00)
  end

  it "should generate the correct default range for daily graphs with include current" do
    widget = FnordMetric::TimelineWidget.new(
      :title => "My Widget",
      :gauges => [@gauge1]
    ) 
    range = widget.default_range(Time.utc(1992,01,13,18,23,23))
    Time.at(range.last).utc.should == Time.utc(1992,01,13,00,00)
    Time.at(range.first).utc.should == Time.utc(1991,12,14,00,00)
  end

  it "should generate the correct default range for hourly graphs with include current" do
    widget = FnordMetric::TimelineWidget.new(
      :title => "My Widget",
      :gauges => [@gauge2]
    ) 
    range = widget.default_range(Time.utc(1992,01,13,18,23,23))
    Time.at(range.last).utc.should == Time.utc(1992,01,13,18,00)
    Time.at(range.first).utc.should == Time.utc(1992,01,12,18,00)
  end

  it "should generate the correct default range for hourly graphs with include current" do
    widget = FnordMetric::TimelineWidget.new(
      :title => "My Widget",
      :include_current => false,
      :gauges => [@gauge2]
    ) 
    range = widget.default_range(Time.utc(1992,01,13,18,23,23))
    Time.at(range.last).utc.should == Time.utc(1992,01,13,17,00)
    Time.at(range.first).utc.should == Time.utc(1992,01,12,17,00)
  end

  it "should generate the correct ticks" do
    widget = FnordMetric::TimelineWidget.new(
      :title => "My Widget",
      :gauges => [@gauge2]
    ) 
    Delorean.time_travel_to(Time.utc(1992,01,13,18,23,23)) do
      Time.at(widget.ticks[0]).utc.should == Time.utc(1992,01,12,18,00)
      Time.at(widget.ticks[1]).utc.should == Time.utc(1992,01,12,19,00)
      Time.at(widget.ticks[-1]).utc.should == Time.utc(1992,01,13,18,00)
      Time.at(widget.ticks[-2]).utc.should == Time.utc(1992,01,13,17,00)
    end
  end

private

  def ranges_should_match!(a, b)
    (a.first.to_i - b.first.to_i).should == 0
    (a.last.to_i - b.last.to_i).should == 0
  end

end