require ::File.expand_path('../spec_helper.rb', __FILE__)
require 'irb'

describe FnordMetric::Util do

  describe "parse_time" do

    it "should parse now" do
      FnordMetric::Util.parse_time("now").should == Time.now.to_i
    end

    it "should parse a timestamp" do
      FnordMetric::Util.parse_time("1360437813").should == 1360437813
    end

    it "should parse -secs" do
      FnordMetric::Util.parse_time("-123").should == Time.now.to_i - 123
    end

    time_specs = {
      "-5s" => 5,
      "-5sec" => 5,
      "-5secs" => 5,
      "-5second" => 5,
      "-5seconds" => 5,
      "-5m" => (60 * 5),
      "-5min" => (60 * 5),
      "-5minute" => (60 * 5),
      "-5minutes" => (60 * 5),
      "-5h" => (3600 * 5),
      "-5hour" => (3600 * 5),
      "-5hours" => (3600 * 5),
      "-5d" => (3600 * 24 * 5),
      "-5day" => (3600 * 24 * 5),
      "-5days" => (3600 * 24 * 5)
    }

    time_specs.each do |ts, ti|
      it "should parse #{ts}" do
        FnordMetric::Util.parse_time(ts).should == Time.now.to_i - ti
      end
    end

  end

end
