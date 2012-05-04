require ::File.expand_path('../spec_helper.rb', __FILE__)

describe "core extensions" do

  include FnordMetric

  describe Symbol do
  	it "should return self when intern was called" do
  	  sym = :fnord
  	  sym.intern.should == sym
  	end
  end

end