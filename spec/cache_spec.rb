require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Cache do

  before(:each) do
    FnordMetric::Cache.destroy_all
  end

  it "should store a cache item with a integer value" do
    FnordMetric::Cache.store!('mykey', 123)
    FnordMetric::Cache.last[:data].should be_a(Hash)
    FnordMetric::Cache.last[:data].keys.should == ["value"]
    FnordMetric::Cache.last[:data]["value"].to_i.should == 123
  end

  it "should store a cache item with a float value" do
    FnordMetric::Cache.store!('mykey', 123.5)
    FnordMetric::Cache.last[:data].should be_a(Hash)
    FnordMetric::Cache.last[:data].keys.should == ["value"]
    FnordMetric::Cache.last[:data]["value"].to_i.should == 123.5
  end

  it "should store a cache item with a hash value" do
    FnordMetric::Cache.store!('mykey', :foobar => "fnord", :blubb => "asd")
    FnordMetric::Cache.last[:data].should be_a(Hash)
    FnordMetric::Cache.last[:data].keys.length.should == 2
    FnordMetric::Cache.last[:data].keys.should include("foobar")
    FnordMetric::Cache.last[:data].keys.should include("blubb")
    FnordMetric::Cache.last[:data]["foobar"].should == "fnord"
    FnordMetric::Cache.last[:data]["blubb"].should == "asd"
  end

  it "should get a cache item with a integer value" do
    FnordMetric::Cache.store!('mykey', 123)
    FnordMetric::Cache.get('mykey').should == 123
  end

  it "should store a cache item with a float value" do
    FnordMetric::Cache.store!('mykey', 42.5)
    FnordMetric::Cache.get('mykey').should == 42.5
  end

  it "should store a cache item with a hash value" do
    FnordMetric::Cache.store!('mykey', :foobar => "fnord", :blubb => "asd")
    FnordMetric::Cache.get('mykey').should be_a(Hash)
    FnordMetric::Cache.get('mykey').keys.length.should == 2
    FnordMetric::Cache.get('mykey').keys.should include("foobar")
    FnordMetric::Cache.get('mykey').keys.should include("blubb")
    FnordMetric::Cache.get('mykey')["foobar"].should == "fnord"
    FnordMetric::Cache.get('mykey')["blubb"].should == "asd"
  end

end
