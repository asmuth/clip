require 'spec_helper'

describe FnordMetric do
  after { FnordMetric.server_configuration = nil }

  it "has default_options" do
    opts = FnordMetric.default_options
    opts[:redis_url].should eq("redis://localhost:6379")
    opts[:redis_prefix].should eq("fnordmetric")
    opts[:inbound_stream].should eq(["0.0.0.0", "1337"])
    opts[:web_interface].should eq(["0.0.0.0", "4242"])
    opts[:web_interface_server].should eq("thin")
    opts[:start_worker].should be_true
    opts[:print_stats].should == 3
    opts[:event_queue_ttl].should == 120
    opts[:event_data_ttl].should == 3600*24*30
    opts[:session_data_ttl].should == 3600*24*30
  end

  it "default_options can be overriden" do
    opts = FnordMetric.default_options({
      :redis_url => "redis://example.com:6379",
      :redis_prefix => "foobar",
      :inbound_stream => ["0.0.0.0", "7331"],
      :web_interface => ["0.0.0.0", "2424"],
      :web_interface_server => "hatetepe",
      :start_worker => false,
      :print_stats => false,
      :event_queue_ttl => 10,
      :event_data_ttl => 20,
      :session_data_ttl => 30,
    })
    opts[:redis_url].should eq("redis://example.com:6379")
    opts[:redis_prefix].should eq("foobar")
    opts[:inbound_stream].should eq(["0.0.0.0", "7331"])
    opts[:web_interface].should eq(["0.0.0.0", "2424"])
    opts[:web_interface_server].should eq("hatetepe")
    opts[:start_worker].should be_false
    opts[:print_stats].should be_false
    opts[:event_queue_ttl].should == 10
    opts[:event_data_ttl].should == 20
    opts[:session_data_ttl].should == 30
  end

  it "merges default, server configuration, and given opts as options" do
    FnordMetric.server_configuration = { :web_interface_server => "hatetepe", :print_stats => true }
    opts = FnordMetric.options(:redis_url => "redis://example.com:6379", :print_stats => false)

    opts[:redis_url].should eq("redis://example.com:6379")
    opts[:redis_prefix].should eq("fnordmetric")
    opts[:web_interface_server].should eq("hatetepe")
    opts[:print_stats].should be_false
  end


end