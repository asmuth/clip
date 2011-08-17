require ::File.expand_path('../spec_helper.rb', __FILE__)

describe "base" do

	it "should create a new data point" do
		old_count = MyMetric.count
		MyMetric.insert(:foo => 1, :bar => 5)
		MyMetric.count.should == old_count + 1
	end

	it "should generate a report" do
		MyMetric.generate_report.should be_a(FnordMetric::Report)
	end

end