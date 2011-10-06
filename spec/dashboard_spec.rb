require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Dashboard do

  before(:each) do
    FnordMetric::Event.destroy_all
  end

  it "should remember it's title" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| }
    dashboard.title.should == 'My Foobar Dashboard'
  end

end