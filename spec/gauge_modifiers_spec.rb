require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::GaugeModifiers do

   it "should increment a non-progressive gauge by 1"
  it "should increment a non-progressive gauge by 5"
  it "should increment a progressive gauge by 1"
  it "should increment a progressive gauge by 5"
  it "should raise an error if incr is called on a 3d gauge"

  it "should increment a field on a three-dimensional gauge by 1"
  it "should increment a field on a three-dimensional gauge by 5"
  it "should raise an error if incr_field is called on a 2d gauge"

end