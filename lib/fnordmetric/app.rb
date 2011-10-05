class FnordMetric::App < Sinatra::Base
	
	@@sessions = Hash.new
	
	use Rack::Reloader, 0
	
	enable :session

	set :haml, :format => :html5 
	set :views, ::File.expand_path('../../../haml', __FILE__)
	set :public, ::File.expand_path('../../../pub', __FILE__)
	
	get '/' do
		haml :app
	end
	
end

