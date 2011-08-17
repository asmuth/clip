require ::File.expand_path('../spec_helper.rb', __FILE__)

describe "base" do

	it "should create a new data point" do
		old_count = MyMetric.count
		MyMetric.insert(:foo => 1, :bar => 5)
		MyMetric.count.should == old_count + 1
	end

	it "should insert a data point in the past" do
		my_time = 23.minutes.ago
		MyMetric.insert(:foo => 1, :time => my_time)
		(MyMetric.last.created_at.to_i - my_time.to_i).should == 0
	end

	it "should generate a report" do
		MyMetric.generate_report.should be_a(FnordMetric::Report)
	end

	it "should generate a history report" do
		MyMetric.delete_all
		MyMetric.insert(:mycount => 5, :time => 2.hours.ago)
		MyMetric.insert(:mycount => 10, :time => 1.hours.ago)
		MyMetric.insert(:mycount => 15)
		report = MyMetric.generate_report(:from => 4.hours.ago, :until => Time.now, :interval => 1.hour)
		report.nodes.length.should == 4
	end

end