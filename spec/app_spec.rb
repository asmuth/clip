require ::File.expand_path('../spec_helper.rb', __FILE__)

include FnordMetric

describe "app" do
  include Rack::Test::Methods

  def app
    @app ||= FnordMetric::App
  end

end