require ::File.expand_path('../spec_helper.rb', __FILE__)

include Fnordmetric

describe "app" do
  include Rack::Test::Methods

  def app
    @app ||= Fnordmetric::App
  end

end