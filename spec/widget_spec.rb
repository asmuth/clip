require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Widget do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  it "should remembe it's own title" do
    widget = FnordMetric::Widget.new(:title => "My Widget")
    widget.title.should == "My Widget"
  end

end