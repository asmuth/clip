describe "app" do
  include Rack::Test::Methods

  def app
    @app ||= Fnordmetric::App
  end

end