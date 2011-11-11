require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::CombineMetric do

  it "should return the block return" do
    metric = FnordMetric.metric('my_event_count', :combine => lambda{ |time_or_range|
      2323
    })
    metric.current.should == 2323
  end

  it "should pass the time_or_range to the bloc" do
    metric = FnordMetric.metric('my_event_count', :combine => lambda{ |time_or_range|
      time_or_range.to_i
    })
    metric.current.should == Time.now.to_i
  end

end